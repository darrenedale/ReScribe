//
// Created by darren on 25/06/2020.
//

#include "progresswidget.h"
#include "ui_progresswidget.h"

ReScribe::ProgressWidget::ProgressWidget(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::ProgressWidget>())
{
    m_ui->setupUi(this);
}

void ReScribe::ProgressWidget::setImage(const QString & image)
{
    m_ui->image->setText(image);
}

QString ReScribe::ProgressWidget::image() const
{
    return m_ui->image->text();
}

QString ReScribe::ProgressWidget::device() const
{
    return m_ui->device->text();
}

void ReScribe::ProgressWidget::setDevice(const QString & device)
{
    m_ui->device->setText(device);
}

int ReScribe::ProgressWidget::progress() const
{
    return m_ui->progressBar->value();
}

void ReScribe::ProgressWidget::setProgress(int progress)
{
    m_ui->progressBar->setValue(progress);
}

ReScribe::ProgressWidget::~ProgressWidget() = default;
