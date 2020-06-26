//
// Created by darren on 26/06/2020.
//

#include "messagewidget.h"
#include "ui_messagewidget.h"

ReScribe::MessageWidget::MessageWidget(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::MessageWidget>())
{
    m_ui->setupUi(this);
    connect(m_ui->closeButton, &QPushButton::clicked, this, &MessageWidget::closeClicked);
}

void ReScribe::MessageWidget::setMessage(const QString & msg)
{
    m_ui->message->setText(msg);
}

QString ReScribe::MessageWidget::message() const
{
    return m_ui->message->text();
}

bool ReScribe::MessageWidget::iconVisible() const {
    return m_ui->icon->isVisible();
}

ReScribe::MessageWidget::~MessageWidget() = default;
