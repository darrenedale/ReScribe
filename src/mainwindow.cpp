//
// Created by darren on 21/06/2020.
//

#include <memory>
#include <QApplication>
#include <QStandardPaths>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <KAuthAction>
#include <KAuthExecuteJob>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace ReScribe;

MainWindow::MainWindow(QWidget * parent)
: QDialog(parent),
  m_ui(std::make_unique<Ui::MainWindow>()),
  m_writeButton(std::make_unique<QPushButton>())
{
    m_ui->setupUi(this);
    m_ui->stack->setCurrentWidget(m_ui->configContainer);

    m_ui->imageSelector->setImageUrl(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/"));

    auto * writeButton = new QPushButton(QIcon::fromTheme("document-edit"), tr("Write"));
    m_ui->controls->addButton(writeButton, QDialogButtonBox::ActionRole);
    connect(writeButton, &QPushButton::clicked, this, &MainWindow::writeImage);
}

MainWindow::~MainWindow() = default;

QUrl MainWindow::imageUrl() const
{
    return  m_ui->imageSelector->imageUrl();
}

void MainWindow::setImageUrl(const QUrl & url)
{
    m_ui->imageSelector->setImageUrl(url);
}

QString MainWindow::imageDescription() const
{
    return m_ui->imageSelector->imageDescription();
}

QString MainWindow::devicePath() const
{
    return m_ui->deviceSelector->devicePath();
}

void MainWindow::writeImage()
{
    // TODO reject if already writing

    if (imageUrl().isEmpty()) {
        QMessageBox::critical(
                this,
                tr("%1 Error").arg(QApplication::applicationDisplayName()),
                tr("You must choose an image file to write.")
        );

        return;
    }

    if (devicePath().isEmpty()) {
        QMessageBox::critical(
                this,
                tr("%1 Error").arg(QApplication::applicationDisplayName()),
                tr("You must choose an device to write to.")
        );

        return;
    }

    if (QMessageBox::Yes != QMessageBox::question(
            this,
            tr("%1 Write Image").arg(QApplication::applicationDisplayName()),
            tr("Are you sure you want to overwrite the device %1?").arg(m_ui->deviceSelector->deviceText())
    )) {
        return;
    }

    KAuth::Action writeAction(QStringLiteral("net.equituk.rescribe.imagewriter.write"));
    writeAction.setHelperId(QStringLiteral("net.equituk.rescribe.imagewriter"));

    // can't find a "never timeout" setting, so give the helper 1 hour to complete
    writeAction.setTimeout(60 * 60 * 1000);

    writeAction.setArguments({
             {QStringLiteral("image"), imageUrl().toLocalFile()},
             {QStringLiteral("device"), devicePath()}
     });

    showWriteProgress();
    m_ui->progressWidget->setImage(imageUrl().toString());
    m_ui->progressWidget->setDevice(devicePath());
    m_ui->progressWidget->setProgress(0);
    auto * writeJob = writeAction.execute();

    connect(writeJob, qOverload<KJob *, unsigned long>(&KAuth::ExecuteJob::percent), [this](KJob *, unsigned long pc) {
        m_ui->progressWidget->setProgress(pc);
        qDebug() << "Progress:" << pc << "%";
    });

    connect(writeJob, &KAuth::ExecuteJob::finished, [this] (KJob * job) {
        auto * writeJob = qobject_cast<KAuth::ExecuteJob *>(job);
        qDebug() << "Job finished" << writeJob->errorString();
        QTimer::singleShot(5000, this, &MainWindow::showConfiguration);
    });

    writeJob->start();
}

void MainWindow::showConfiguration()
{
    m_ui->stack->setCurrentWidget(m_ui->configContainer);
    m_writeButton->setEnabled(true);
}

void MainWindow::showWriteProgress()
{
    m_ui->stack->setCurrentWidget(m_ui->progressContainer);
    m_writeButton->setEnabled(false);
}
