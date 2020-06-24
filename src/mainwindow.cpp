//
// Created by darren on 21/06/2020.
//

#include <memory>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <KAuthAction>
#include <KAuthExecuteJob>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace ReScribe;

MainWindow::MainWindow(QWidget * parent)
: QDialog(parent),
  m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);

    auto * writeButton = m_ui->controls->addButton(tr("Write"), QDialogButtonBox::ActionRole);
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

    KAuth::Action writeAction(QStringLiteral("net.equituk.rescribe.imagewriter.write"));
    writeAction.setHelperId(QStringLiteral("net.equituk.rescribe.imagewriter"));

    writeAction.setArguments({
             {QStringLiteral("image"), imageUrl().toLocalFile()},
             {QStringLiteral("device"), devicePath()}
     });

    // TODO job dialogue
    auto * writeJob = writeAction.execute();
    connect(writeJob, qOverload<KJob *, unsigned long>(&KAuth::ExecuteJob::percent), [this](KJob *, unsigned long pc) {
        qDebug() << "Progress:" << pc << "%";
    });
    connect(writeJob, &KAuth::ExecuteJob::finished, [this](KJob * job) {
        auto * writeJob = qobject_cast<KAuth::ExecuteJob *>(job);
        qDebug() << "Job finished" << writeJob->errorString();
    });
    writeJob->start();
}
