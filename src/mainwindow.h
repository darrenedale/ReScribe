//
// Created by darren on 21/06/2020.
//

#ifndef RESCRIBE_MAINWINDOW_H
#define RESCRIBE_MAINWINDOW_H

#include <QDialog>

class QPushButton;

namespace Ui {
    class MainWindow;
}

namespace ReScribe {
    class MainWindow : public QDialog
    {
        Q_OBJECT

        public:
            enum class State
            {
                Ready = 0,
                Downloading = 1,
                Writing = 2,
            };

            MainWindow(QWidget * parent = nullptr);
            MainWindow(const MainWindow &) = delete;
            MainWindow(MainWindow &&) = delete;
            virtual ~MainWindow();

            void operator=(const MainWindow &) = delete;
            void operator=( MainWindow && ) = delete;

            QUrl imageUrl() const;
            void setImageUrl(const QUrl & url);

            QString imageDescription() const;

            QString devicePath() const;
            QString deviceDescription() const;

            bool decompress() const;
            void setDecompress(bool decompress);

            void writeImage();

            void showConfigurationWidget();
            void showProgressWidget();

            State state() const
            {
                return m_state;
            }

        private:
            /**
             * Helper to write an image sourced from a remote URL.
             *
             * @param url
             */
            void writeRemoteImage(const QUrl & url);

            /**
             * Helper to write an image sourced from a local file.
             *
             * If provided the fileName is the name of the locally-downloaded version of a remote URL. The provided
             * filename will not be used for display purposes, the imageUrl() will be used. If the fileName is not
             * provided, it is assumed that imageUrl() is a local file and that file is used.
             *
             * @param fileName QString optional local image to write, overriding the imageUrl().
             */
            void writeLocalImage(QString fileName = QStringLiteral());

            std::unique_ptr<Ui::MainWindow> m_ui;

            State m_state;
    };
}

#endif //RESCRIBE_MAINWINDOW_H
