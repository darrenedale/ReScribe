//
// Created by darren on 21/06/2020.
//

#include <memory>
#include <QStringBuilder>
#include <QApplication>
#include <QStandardPaths>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QTemporaryFile>
#include <QDebug>
#include <KIO/FileCopyJob>
#include <KAuthAction>
#include <KAuthExecuteJob>
#include "mainwindow.h"
#include "application.h"
#include "diskimageinfofactory.h"
#include "diskimageinfo.h"
#include "sizetextgenerator.h"
#include "writeconfirmationdialogue.h"
#include "ui_mainwindow.h"

namespace {
    static constexpr const int Timeout = 60 * 60 * 1000;
}

using namespace ReScribe;

MainWindow::MainWindow(QWidget * parent)
: QDialog(parent),
  m_ui(std::make_unique<Ui::MainWindow>()),
  m_state(State::Ready)
{
    m_ui->setupUi(this);
    m_ui->imageSelector->setImageUrl(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/"));

    connect(m_ui->back, &QPushButton::clicked, this, &MainWindow::showConfigurationWidget);
    connect(m_ui->write, &QPushButton::clicked, this, &MainWindow::writeImage);
    connect(m_ui->close, &QPushButton::clicked, rsApp, &Application::quit);

    showConfigurationWidget();
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
    if (State::Ready != state()) {
        rsApp->showNotification(tr("A disk image is currently being downloaded or written. Wait for that operation to finish (or cancel it) before starting another."));
        return;
    }

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
                tr("You must choose a device to write to.")
        );

        return;
    }

    WriteConfirmationDialogue dlg(this);
    dlg.setDevice(deviceDescription());

    if (imageUrl().isLocalFile()) {
        auto info = DiskImageInfoFactory::create(imageUrl().toLocalFile());

        if (info) {
            if (info->infoIsAvailable()) {
                dlg.setDiskImage(info->type() % QStringLiteral(": ") % info->description());
            } else {
                dlg.setDiskImage(imageUrl().toString() % QStringLiteral(" [") % info->type() % QStringLiteral("]"));
            }
        } else {
            dlg.setDiskImage(imageUrl().toString());
        }
    } else {
        dlg.setDiskImage(imageUrl().toString());
    }

    connect(&dlg, &WriteConfirmationDialogue::confirmed, [this] () {
        if (imageUrl().isLocalFile()) {
            writeLocalImage();
        } else {
            writeRemoteImage(imageUrl());
        }
    });

    dlg.exec();
}

void MainWindow::showConfigurationWidget()
{
    m_ui->stack->setCurrentWidget(m_ui->configContainer);
    m_ui->decompress->setEnabled(true);
    m_ui->write->setEnabled(true);
    m_ui->close->setEnabled(true);
    m_ui->back->setEnabled(false);
}

void MainWindow::showProgressWidget()
{
    m_ui->stack->setCurrentWidget(m_ui->progressWidget);
    m_ui->decompress->setEnabled(false);
    m_ui->write->setEnabled(false);
    m_ui->close->setEnabled(false);
}

template<typename NextFn>
void MainWindow::downloadChecksum(const QUrl & url, NextFn && then)
{
    m_state = State::Downloading;
    showProgressWidget();

    auto * checksumFile = new QTemporaryFile(QStringLiteral("rescribe-remote-image-checksum-XXXXXX"), this);
    checksumFile->open();
    KIO::FileCopyJob * downloadJob = KIO::file_copy(imageUrl(), QUrl::fromLocalFile(checksumFile->fileName()), -1, KIO::JobFlag::Overwrite | KIO::JobFlag::HideProgressInfo);

    auto cancelConnection = connect(m_ui->progressWidget, &ProgressWidget::cancelClicked, [downloadJob] () {
        downloadJob->kill(KJob::KillVerbosity::EmitResult);
    });

    m_ui->progressWidget->setCancelButtonEnabled(true);
    m_ui->progressWidget->setProgress(0);
    m_ui->progressWidget->setImage(url.toString());
    m_ui->progressWidget->setDevice(deviceDescription());
    m_ui->progressWidget->setStatus(tr("Downloading checksum file..."));

    connect(downloadJob, &KIO::FileCopyJob::finished, [this, downloadJob, checksumFile, url, cancelConnection = std::move(cancelConnection), then = std::forward(then)] (KJob * job) {
        disconnect(cancelConnection);
        downloadJob->deleteLater();
        checksumFile->close();

        if (0 == job->error()) {
            then(url, checksumFile->fileName());
            // NOTE leave parent to remove temp file as we need it as long as the write job is in progress
        } else {
            qDebug() << job->error() << job->errorString();
            m_ui->progressWidget->setStatus(tr("The disk image could not be downloaded."));
            rsApp->showNotification(tr("The disk image could not be downloaded."));
            delete checksumFile;
            m_ui->progressWidget->setCancelButtonEnabled(false);
            m_ui->close->setEnabled(true);
            m_ui->back->setEnabled(true);
            m_state = State::Ready;
        }
    });
}

void MainWindow::writeRemoteImage(const QUrl & url)
{
    m_state = State::Downloading;
    showProgressWidget();

    auto * imageFile = new QTemporaryFile(QStringLiteral("rescribe-remote-image-XXXXXX"), this);
    imageFile->open();
    KIO::FileCopyJob * downloadJob = KIO::file_copy(imageUrl(), QUrl::fromLocalFile(imageFile->fileName()), -1, KIO::JobFlag::Overwrite | KIO::JobFlag::HideProgressInfo);

    auto cancelConnection = connect(m_ui->progressWidget, &ProgressWidget::cancelClicked, [downloadJob] () {
        downloadJob->kill(KJob::KillVerbosity::EmitResult);
    });

    m_ui->progressWidget->setCancelButtonEnabled(true);
    m_ui->progressWidget->setProgress(0);
    m_ui->progressWidget->setImage(url.toString());
    m_ui->progressWidget->setDevice(deviceDescription());
    m_ui->progressWidget->setStatus(tr("Downloading disk image..."));

    connect(downloadJob, qOverload<KJob *, unsigned long>(&KIO::FileCopyJob::percent), m_ui->progressWidget, [this] (KJob *, unsigned long pc) {
        m_ui->progressWidget->setProgress(static_cast<int>(pc / 2));
    });

    connect(downloadJob, &KIO::FileCopyJob::processedSize, m_ui->progressWidget, [this, downloadJob] (KJob *, qulonglong bytes) {
        auto sizeText = SizeTextGenerator();
        m_ui->progressWidget->setStatus(
                tr("%1 of %2 downloaded")
                .arg(sizeText.setSize(bytes).text<QString>())
                .arg(sizeText.setSize(downloadJob->totalAmount(KJob::Unit::Bytes)).text<QString>())
                );
    });

    connect(downloadJob, &KIO::FileCopyJob::finished, [this, downloadJob, imageFile, cancelConnection = std::move(cancelConnection)] (KJob * job) {
        disconnect(cancelConnection);
        downloadJob->deleteLater();
        imageFile->close();

        if (0 == job->error()) {
            writeLocalImage(imageFile->fileName());
            // NOTE leave parent to remove temp file as we need it as long as the write job is in progress
        } else {
            qDebug() << job->error() << job->errorString();
            m_ui->progressWidget->setStatus(tr("The disk image could not be downloaded: %1.").arg((job->errorString())));
            rsApp->showNotification(tr("The disk image could not be downloaded: %1.").arg(job->errorString()));
            delete imageFile;
            m_ui->progressWidget->setCancelButtonEnabled(false);
            m_ui->close->setEnabled(true);
            m_ui->back->setEnabled(true);
            m_ui->decompress->setEnabled(true);
            m_state = State::Ready;
        }
    });
}

void MainWindow::writeLocalImage(QString fileName)
{
    m_state = State::Writing;

    if (fileName.isEmpty()) {
        fileName = imageUrl().toLocalFile();
    }

    KAuth::Action writeAction(QStringLiteral("net.equituk.rescribe.imagewriter.write"));
    writeAction.setHelperId(QStringLiteral("net.equituk.rescribe.imagewriter"));

    // can't find a "never timeout" setting, so give the helper 1 hour to complete
    writeAction.setTimeout(Timeout);

    writeAction.setArguments({
        {QStringLiteral("image"),  fileName},
        {QStringLiteral("device"), devicePath()},
        {QStringLiteral("decompress"), decompress()},
    });

    showProgressWidget();
    m_ui->progressWidget->setDevice(deviceDescription());
    auto * writeJob = writeAction.execute();
    m_ui->progressWidget->setCancelButtonEnabled(true);

    auto cancelConnection = connect(m_ui->progressWidget, &ProgressWidget::cancelClicked, [writeJob] () {
        qDebug() << "Killing write job";
        writeJob->kill(KJob::KillVerbosity::EmitResult);
    });

    if (imageUrl().isLocalFile()) {
        m_ui->progressWidget->setProgress(0);
        m_ui->progressWidget->setImage(imageUrl().toLocalFile());

        connect(writeJob, qOverload<KJob *, unsigned long>(&KAuth::ExecuteJob::percent),
                [this](KJob *, unsigned long pc) {
                    m_ui->progressWidget->setProgress(pc);
                });
    } else {
        // writing remote image, so start at 50% on the assumption that the first 50% was downloading the disk image
        m_ui->progressWidget->setProgress(50);
        m_ui->progressWidget->setImage(imageUrl().toString());

        connect(writeJob, qOverload<KJob *, unsigned long>(&KAuth::ExecuteJob::percent),
                [this](KJob *, unsigned long pc) {
                    m_ui->progressWidget->setProgress(50 + pc / 2);
                });
    }

    connect(writeJob, &KAuth::ExecuteJob::newData, [this] (const QVariantMap & data) {
        if (!data.contains(QStringLiteral("bytesWritten")) || !data.contains(QStringLiteral("totalBytes"))) {
            qDebug() << "Received unrecognised data from KAuth::ExecuteJob:" << data;
            return;
        }

        // NOTE totalBytes will be -1 if the total size is not known (i.e. we're decompressing on-the-fly)
        auto bytesWritten = data["bytesWritten"].value<qint64>();
        auto totalBytes = data["totalBytes"].value<qint64>();
        auto sizeText = SizeTextGenerator();

        if (0 < totalBytes) {
            m_ui->progressWidget->setStatus(
                    tr("%1 of %2 written")
                            .arg(sizeText.setSize(bytesWritten).text<QString>())
                            .arg(sizeText.setSize(totalBytes).text<QString>())
            );
        } else {
            m_ui->progressWidget->setStatus(tr("%1 written").arg(sizeText.setSize(bytesWritten).text<QString>()));
        }
    });

    connect(writeJob, &KAuth::ExecuteJob::finished, [this, cancelConnection = std::move(cancelConnection)] (KJob * job) {
        disconnect(cancelConnection);
        auto * writeJob = qobject_cast<KAuth::ExecuteJob *>(job);

        if (writeJob->error()) {
            qDebug() << "Error writing image: [" << writeJob->error() << "]" << writeJob->errorString();
            m_ui->progressWidget->setStatus(tr("The image was not written: %1.").arg(writeJob->errorString()));
            rsApp->showNotification(tr("The image was not written: %1.").arg(writeJob->errorString()));
        } else {
            m_ui->progressWidget->setStatus(tr("The image was written successfully."));
            rsApp->showNotification(tr("The image was written successfully."));
        }

        m_ui->progressWidget->setCancelButtonEnabled(false);
        m_ui->close->setEnabled(true);
        m_ui->back->setEnabled(true);
        m_state = State::Ready;
    });

    writeJob->start();
}

QString MainWindow::deviceDescription() const
{
    return m_ui->deviceSelector->deviceText();
}

bool MainWindow::decompress() const {
    return m_ui->decompress->isChecked();
}

void MainWindow::setDecompress(bool decompress)
{
    return m_ui->decompress->setChecked(decompress);
}
