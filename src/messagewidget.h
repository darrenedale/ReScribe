//
// Created by darren on 26/06/2020.
//

#ifndef RESCRIBE_MESSAGEWIDGET_H
#define RESCRIBE_MESSAGEWIDGET_H

#include <memory>
#include <QWidget>
#include <QUrl>

namespace Ui {
    class MessageWidget;
}

namespace ReScribe
{
    class MessageWidget : public QWidget
    {
    Q_OBJECT

    public:
        explicit MessageWidget(QWidget * parent = nullptr);
        MessageWidget(const MessageWidget &) = delete;
        MessageWidget(MessageWidget &&) = delete;
        virtual ~MessageWidget();

        void operator=(const MessageWidget &) = delete;
        void operator=(MessageWidget && ) = delete;

        QString message() const;
        void setMessage(const QString & image);

        bool iconVisible() const;
        void hideIcon();
        void showIcon();
        QIcon icon() const;
        void setIcon(const QIcon & icon);

        bool titleVisible() const;
        void hideTitle();
        void showTitle();
        QString title() const;
        void setTitle(const QString & title);

    Q_SIGNALS:
        void closeClicked();

    private:
        std::unique_ptr<Ui::MessageWidget> m_ui;
    };
}

#endif //RESCRIBE_MESSAGEWIDGET_H
