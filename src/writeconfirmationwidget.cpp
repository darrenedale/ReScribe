//
// Created by darren on 30/06/2020.
//

#include <QIcon>
#include "writeconfirmationwidget.h"
#include "ui_writeconfirmationwidget.h"

using namespace ReScribe;

WriteConfirmationWidget::WriteConfirmationWidget(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::WriteConfirmationWidget>())
{
    m_ui->setupUi(this);

    m_ui->icon->setPixmap(QIcon::fromTheme(QStringLiteral("dialog-question")).pixmap(m_ui->icon->baseSize().width()));
}

void WriteConfirmationWidget::setDiskImage(const QString & diskImage)
{
    m_ui->diskImage->setText(diskImage);
}

QString WriteConfirmationWidget::diskImage() const
{
    return m_ui->diskImage->text();
}

void WriteConfirmationWidget::setDevice(const QString & device)
{
    m_ui->device->setText(device);
}

QString WriteConfirmationWidget::device() const
{
    return m_ui->device->text();
}

WriteConfirmationWidget::~WriteConfirmationWidget() = default;
