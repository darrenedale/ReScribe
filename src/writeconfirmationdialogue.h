//
// Created by darren on 30/06/2020.
//

#ifndef RESCRIBE_WRITECONFIRMATIONDIALOGUE_H
#define RESCRIBE_WRITECONFIRMATIONDIALOGUE_H

#include <memory>
#include <QDialog>

namespace Ui {
    class WriteConfirmationDialogue;
}

namespace ReScribe {
    class WriteConfirmationDialogue : public QDialog
    {
    Q_OBJECT

    public:
        WriteConfirmationDialogue(QWidget * parent = nullptr);
        WriteConfirmationDialogue(const WriteConfirmationDialogue &) = delete;
        WriteConfirmationDialogue(WriteConfirmationDialogue &&) = delete;
        void operator=(const WriteConfirmationDialogue &) = delete;
        void operator=(WriteConfirmationDialogue &&) = delete;
        ~WriteConfirmationDialogue() override;

        void setDiskImage(const QString & diskImage);
        QString diskImage() const;

        void setDevice(const QString & device);
        QString device() const;

    Q_SIGNALS:
        void confirmed();
        void declined();

    private:
        std::unique_ptr<Ui::WriteConfirmationDialogue> m_ui;
    };
}

#endif //RESCRIBE_WRITECONFIRMATIONDIALOGUE_H
