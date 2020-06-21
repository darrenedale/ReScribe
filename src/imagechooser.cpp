//
// Created by darren on 21/06/2020.
//

#include "imagechooser.h"
#include "ui_imagechooser.h"

using namespace ReScribe;

ImageChooser::ImageChooser(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::ImageChooser>())
{
    m_ui->setupUi(this);
    connect(m_ui->imageSelector, &KUrlComboRequester::urlSelected, this, &ImageChooser::imageUrlChanged);
}

ImageChooser::~ImageChooser() = default;

QUrl ImageChooser::imageUrl() const
{
    return m_ui->imageSelector->url();
}

void ImageChooser::setImageDescription(const QString & description)
{
    m_ui->imageDescription->setText(description);
}

QString ImageChooser::imageDescription() const
{
    return m_ui->imageDescription->text();
}

void ImageChooser::setImageUrl(const QUrl & url)
{
    m_ui->imageSelector->setUrl(url);
}
