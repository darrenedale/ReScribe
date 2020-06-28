//
// Created by darren on 25/06/2020.
//

#include "progresswidget.h"
#include "sizetextgenerator.h"
#include "ui_progresswidget.h"

namespace {
    static constexpr const int IconSize = 32;
}

using namespace ReScribe;

ProgressWidget::ProgressWidget(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::ProgressWidget>())
{
    m_ui->setupUi(this);
    
    m_ui->imageIcon->setPixmap(QIcon::fromTheme(QStringLiteral("application-x-raw-disk-image")).pixmap(IconSize));
    m_ui->deviceIcon->setPixmap(QIcon::fromTheme(QStringLiteral("drive-removable-media")).pixmap(IconSize));

    connect(m_ui->cancel, &QAbstractButton::clicked, this, &ProgressWidget::cancelClicked);
}

void ProgressWidget::setImage(const QString & image)
{
    m_ui->image->setText(image);
}

QString ProgressWidget::image() const
{
    return m_ui->image->text();
}

QString ProgressWidget::device() const
{
    return m_ui->device->text();
}

void ProgressWidget::setDevice(const QString & device)
{
    m_ui->device->setText(device);
}

int ProgressWidget::progress() const
{
    return m_ui->progressBar->value();
}

void ProgressWidget::setProgress(int progress)
{
    m_ui->progressBar->setValue(progress);
}

void ProgressWidget::setStatus(const QString & status)
{
    m_ui->status->setText(status);
}

void ProgressWidget::setCancelButtonEnabled(bool enabled)
{
    m_ui->cancel->setEnabled(enabled);
}

ProgressWidget::~ProgressWidget() = default;
