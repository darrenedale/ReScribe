//
// Created by darren on 30/06/2020.
//

#ifndef RESCRIBE_WRITECONFIRMATIONWIDGET_H
#define RESCRIBE_WRITECONFIRMATIONWIDGET_H

#include <memory>
#include <QWidget>

namespace Ui {
    class WriteConfirmationWidget;
}

namespace ReScribe {
    class WriteConfirmationWidget : public QWidget
    {
        Q_OBJECT

    public:
        WriteConfirmationWidget(QWidget * parent = nullptr);
        WriteConfirmationWidget(const WriteConfirmationWidget &) = delete;
        WriteConfirmationWidget(WriteConfirmationWidget &&) = delete;
        void operator=(const WriteConfirmationWidget &) = delete;
        void operator=(WriteConfirmationWidget &&) = delete;
        ~WriteConfirmationWidget() override;

        void setDiskImage(const QString & diskImage);
        QString diskImage() const;

        void setDevice(const QString & device);
        QString device() const;

    private:
        std::unique_ptr<Ui::WriteConfirmationWidget> m_ui;
    };
}

#endif //RESCRIBE_WRITECONFIRMATIONWIDGET_H
