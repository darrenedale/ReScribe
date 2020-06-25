//
// Created by darren on 21/06/2020.
//

#ifndef RESCRIBE_DEVICECHOOSER_H
#define RESCRIBE_DEVICECHOOSER_H

#include <memory>
#include <QWidget>

namespace Solid
{
    class StorageDrive;
    class Device;
}

namespace Ui {
    class DeviceChooser;
}

namespace ReScribe
{
    class DeviceChooser : public QWidget
    {
        Q_OBJECT

    public:
        DeviceChooser(QWidget * parent = nullptr);
        DeviceChooser(const DeviceChooser &) = delete;
        DeviceChooser(DeviceChooser &&) = delete;
        virtual ~DeviceChooser();

        void operator=(const DeviceChooser &) = delete;
        void operator=(DeviceChooser && ) = delete;

        QString devicePath() const;
        QString deviceText() const;

        Q_SIGNALS:
            void deviceChanged(const QString & deviceFile);

    protected:
        void refreshDeviceList();
        QString lastSelected() const;

    private:
        static bool canUseDrive(const Solid::StorageDrive & drive);
        static std::optional<Solid::Device> partitionTableVolume(const Solid::Device & driveDevice);
        static QString generateSizeText(qulonglong bytes);
        static QString generateDeviceText(const Solid::Device & volumeDevice);
        void updateLastSelected(int idx);

        std::unique_ptr<Ui::DeviceChooser> m_ui;
        QString m_lastSelectedDevice;
    };
}

#endif //RESCRIBE_DEVICECHOOSER_H
