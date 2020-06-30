//
// Created by darren on 30/06/2020.
//

#include "writeconfirmationdialogue.h"
#include "ui_writeconfirmationdialogue.h"

using namespace ReScribe;

WriteConfirmationDialogue::WriteConfirmationDialogue(QWidget * parent)
: QDialog(parent),
  m_ui(std::make_unique<Ui::WriteConfirmationDialogue>())
{
    m_ui->setupUi(this);

    connect(m_ui->controls, &QDialogButtonBox::accepted, this, &WriteConfirmationDialogue::confirmed);
    connect(m_ui->controls, &QDialogButtonBox::rejected, this, &WriteConfirmationDialogue::declined);
}


void WriteConfirmationDialogue::setDiskImage(const QString & diskImage)
{
    m_ui->widget->setDiskImage(diskImage);
}

QString WriteConfirmationDialogue::diskImage() const
{
    return m_ui->widget->diskImage();
}

void WriteConfirmationDialogue::setDevice(const QString & device)
{
    m_ui->widget->setDevice(device);
}

QString WriteConfirmationDialogue::device() const
{
    return m_ui->widget->device();
}

WriteConfirmationDialogue::~WriteConfirmationDialogue() = default;
