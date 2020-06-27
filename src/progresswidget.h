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
        enum class Stage {
            Download = 0,
            Write = 1,
        };

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

        qint64 writtenBytes() const {
            return m_writtenBytes;
        }

        qint64 totalBytes() const {
            return m_totalBytes;
        }

        void setBytes(quint64 written, quint64 total);
        void setTotalBytes(qint64 total) {
            setBytes(writtenBytes(), total);
        }

        void setWrittenBytes(quint64 written) {
            setBytes(written, totalBytes());
        }

        void setStatus(const QString & status);

    private:
        std::unique_ptr<Ui::ProgressWidget> m_ui;
        qint64 m_writtenBytes;
        qint64 m_totalBytes;
    };
}

#endif //RESCRIBE_PROGRESSWIDGET_H
