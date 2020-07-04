//
// Created by darren on 21/06/2020.
//

#include <QUrl>
#include <QStringBuilder>
#include <QFileDialog>
#include "imagechooser.h"
#include "diskimageinfofactory.h"
#include "diskimageinfo.h"
#include "ui_imagechooser.h"

using namespace ReScribe;

ImageChooser::ImageChooser(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::ImageChooser>())
{
    m_ui->setupUi(this);

    m_ui->icon->setPixmap(QIcon::fromTheme("application-x-raw-disk-image").pixmap(m_ui->icon->baseSize().width()));

    connect(m_ui->chooseLocalFile, &QToolButton::clicked, this, &ImageChooser::chooseLocalFile);
    connect(m_ui->imageUrl, &QComboBox::editTextChanged, [this] (const QString & text) {
        auto url = QUrl(text);
        Q_EMIT imageUrlChanged(url);

        if (url.isLocalFile()) {
            auto info = DiskImageInfoFactory::create(url.toLocalFile());

            if (info) {
                if (info->infoIsAvailable()) {
                    setImageDescription(info->type() % QStringLiteral(": ") % info->description());
                } else {
                    setImageDescription(info->type());
                }
            } else {
                setImageDescription(tr("Unrecognised disk image type."));
            }
        } else {
            setImageDescription(tr("Description is not available for remote disk images."));
        }
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
    auto url = QFileDialog::getOpenFileUrl(this, tr("Choose a disk image file"), imageUrl(),"Disk image files (*.iso *.img)\nCompressed disk image files (*.iso.gz *.iso.bz *.iso.xz *.iso.zip *.img *.img.gz *.img.bz *.img.xz *.img.zip)\nAll files (*)");

    if (url.isEmpty()) {
        return;
    }

    setImageUrl(url);
}
