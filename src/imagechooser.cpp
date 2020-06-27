//
// Created by darren on 21/06/2020.
//

#include <QUrl>
#include <QFileDialog>
#include "imagechooser.h"
#include "ui_imagechooser.h"

using namespace ReScribe;

ImageChooser::ImageChooser(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::ImageChooser>())
{
    m_ui->setupUi(this);
    connect(m_ui->chooseLocalFile, &QToolButton::clicked, this, &ImageChooser::chooseLocalFile);
    connect(m_ui->imageUrl, &QComboBox::editTextChanged, [this] (const QString & text) {
        Q_EMIT imageUrlChanged(QUrl(text));
    });
}

ImageChooser::~ImageChooser() = default;

QUrl ImageChooser::imageUrl() const
{
    auto ret = QUrl(m_ui->imageUrl->currentText());

    if (!ret.isValid()) {
        return {};
    }

    return ret;
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
    m_ui->imageUrl->setEditText(url.toString());
}

void ImageChooser::chooseLocalFile()
{
    auto url = QFileDialog::getOpenFileUrl(this, tr("Choose a disk image file"), imageUrl(),"Disk image files (*.iso *.img)\nAll files (*)");

    if (url.isEmpty()) {
        return;
    }

    setImageUrl(url);
}
