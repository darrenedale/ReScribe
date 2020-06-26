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

            void writeImage();

            void showConfiguration();
            void showWriteProgress();
            void showMessage(const QString & msg = {});

        private:
            std::unique_ptr<Ui::MainWindow> m_ui;
            std::unique_ptr<QPushButton> m_writeButton;
    };
}

#endif //RESCRIBE_MAINWINDOW_H
