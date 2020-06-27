//
// Created by darren on 21/06/2020.
//

#ifndef RESCRIBE_IMAGECHOOSER_H
#define RESCRIBE_IMAGECHOOSER_H

#include <memory>
#include <QWidget>
#include <QUrl>

namespace Ui {
    class ImageChooser;
}

namespace ReScribe
{
    class ImageChooser : public QWidget
    {
        Q_OBJECT

        public:
            ImageChooser(QWidget * parent = nullptr);
            ImageChooser(const ImageChooser &) = delete;
            ImageChooser(ImageChooser &&) = delete;
            virtual ~ImageChooser();

            void operator=(const ImageChooser &) = delete;
            void operator=(ImageChooser && ) = delete;

            QUrl imageUrl() const;
            void setImageUrl(const QUrl & url);

            QString imageDescription() const;
            void setImageDescription(const QString & description);

        Q_SIGNALS:
            void imageUrlChanged(QUrl);

        private:
            void chooseLocalFile();
            std::unique_ptr<Ui::ImageChooser> m_ui;
    };
}

#endif //RESCRIBE_IMAGECHOOSER_H
