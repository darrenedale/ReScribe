//
// Created by darren on 21/06/2020.
//

#ifndef RESCRIBE_PROGRESSWIDGET_H
#define RESCRIBE_PROGRESSWIDGET_H

#include <memory>
#include <QWidget>
#include <QUrl>

namespace Ui {
    class ProgressWidget;
}

namespace ReScribe
{
    class ProgressWidget : public QWidget
    {
    Q_OBJECT

    public:
        ProgressWidget(QWidget * parent = nullptr);
        ProgressWidget(const ProgressWidget &) = delete;
        ProgressWidget(ProgressWidget &&) = delete;
        virtual ~ProgressWidget();

        void operator=(const ProgressWidget &) = delete;
        void operator=(ProgressWidget && ) = delete;

        QString image() const;
        void setImage(const QString & image);

        QString device() const;
        void setDevice(const QString & device);

        int progress() const;
        void setProgress(int progress);

    private:
        std::unique_ptr<Ui::ProgressWidget> m_ui;
    };
}

#endif //RESCRIBE_PROGRESSWIDGET_H
