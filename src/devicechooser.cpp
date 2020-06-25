//
// Created by darren on 21/06/2020.
//

#include <iostream>
#include <solid/solidnamespace.h>
#include <solid/devicenotifier.h>
#include <solid/device.h>
#include <solid/block.h>
#include <solid/storagedrive.h>
#include <solid/storagevolume.h>
#include <QDebug>
#include <QStringBuilder>
#include "devicechooser.h"
#include "ui_devicechooser.h"

using namespace ReScribe;
using Solid::DeviceNotifier;
using Solid::DeviceInterface;
using Solid::Device;
using Solid::StorageDrive;
using Solid::StorageVolume;
using Solid::Block;

namespace {
    static constexpr const int DeviceFileRole = Qt::UserRole;
}

DeviceChooser::DeviceChooser(QWidget * parent)
: QWidget(parent),
  m_ui(std::make_unique<Ui::DeviceChooser>())
{
    m_ui->setupUi(this);
    connect(m_ui->device, qOverload<int>(&QComboBox::currentIndexChanged), [this](int idx) {
        updateLastSelected(idx);
        Q_EMIT deviceChanged(devicePath());
    });
    connect(DeviceNotifier::instance(), &DeviceNotifier::deviceAdded, this, &DeviceChooser::refreshDeviceList);
    connect(DeviceNotifier::instance(), &DeviceNotifier::deviceRemoved, this, &DeviceChooser::refreshDeviceList);
    refreshDeviceList();
}

DeviceChooser::~DeviceChooser() = default;

QString DeviceChooser::devicePath() const
{
    return m_ui->device->currentData(DeviceFileRole).toString();
}

QString DeviceChooser::deviceText() const
{
    return m_ui->device->currentText();
}

bool DeviceChooser::canUseDrive(const StorageDrive & drive)
{
    auto * block = reinterpret_cast<const Solid::Block *>(&drive);
    std::cerr << "Drive "
        << block->device().toUtf8().constData()
        << " is a "
        << (drive.isRemovable() ? "removable " : "fixed ")
        << (drive.isHotpluggable() ? "hot-pluggable " : "");

    switch (drive.bus()) {
        case StorageDrive::Bus::Usb:
            std::cerr << "USB ";
            break;

        case StorageDrive::Bus::Ide:
            std::cerr << "IDE ";
            break;

        case StorageDrive::Bus::Ieee1394:
            std::cerr << "IEEE1394 ";
            break;

        case StorageDrive::Bus::Sata:
            std::cerr << "SATA ";
            break;

        case StorageDrive::Bus::Scsi:
            std::cerr << "SCSI ";
            break;

        case StorageDrive::Bus::Platform:
            std::cerr << "platform-bus ";
            break;
    }

    switch (drive.driveType()) {
        case StorageDrive::DriveType::HardDisk:
            std::cerr << "Hard Disk\n" << std::flush;
            break;

        case StorageDrive::DriveType::CdromDrive:
            std::cerr << "CD ROM\n" << std::flush;
            break;

        case StorageDrive::DriveType::Floppy:
            std::cerr << "Floppy Disk\n" << std::flush;
            break;

        case StorageDrive::DriveType::Tape:
            std::cerr << "Tape Drive\n" << std::flush;
            break;

        case StorageDrive::DriveType::CompactFlash:
            std::cerr << "Compact Flash Drive\n" << std::flush;
            break;

        case StorageDrive::DriveType::SdMmc:
            std::cerr << "SD/MMC Drive\n" << std::flush;
            break;

        case StorageDrive::DriveType::Xd:
            std::cerr << "Xd Drive\n" << std::flush;
            break;

        case StorageDrive::DriveType::SmartMedia:
            std::cerr << "Smart Media Drive\n" << std::flush;
            break;

        case StorageDrive::DriveType::MemoryStick:
            std::cerr << "Memory Stick\n" << std::flush;
            break;

        default:
            std::cerr << "unknown drive type (0x" << std::hex << drive.driveType() << ")\n" << std::flush;
            break;
    }

    return Solid::StorageDrive::Bus::Usb == drive.bus() && drive.isRemovable();
}

std::optional<Device> DeviceChooser::partitionTableVolume(const Device & driveDevice)
{
    const auto * drive = driveDevice.as<StorageDrive>();

    for (auto device : Solid::Device::listFromType(DeviceInterface::StorageVolume, driveDevice.udi())) {
        auto * volume = device.as<StorageVolume>();

        if (!volume) {
            qDebug() << "Device" << device.udi() << "could not be cast to StorageVolume";
            continue;
        }

        if (StorageVolume::UsageType::PartitionTable == volume->usage()) {
            return std::move(device);
        }
    }

    qDebug() << "Failed to find the partition volume for" << driveDevice.vendor() <<  driveDevice.product();
    return {};
}

QString DeviceChooser::generateSizeText(qulonglong bytes)
{
    qulonglong threshold = 1;

    if (threshold * 1024 > bytes) {
        return QStringLiteral("%1 bytes").arg(bytes);
    }

    threshold *= 1024;

    if (threshold * 1024 > bytes) {
        return QStringLiteral("%1KiB").arg(bytes / threshold);
    }

    threshold *= 1024;

    if (threshold * 1024 > bytes) {
        return QStringLiteral("%1 MiB").arg(static_cast<double>(bytes) / threshold, 0, 'f', 1, '0');
    }

    threshold *= 1024;

    if (threshold * 1024 > bytes) {
        return QStringLiteral("%1 GiB").arg(static_cast<double>(bytes) / threshold, 0, 'f', 1, '0');
    }

    threshold *= 1024;
    return QStringLiteral("%1 TiB").arg(static_cast<double>(bytes) / threshold, 0, 'f', 1, '0');
}

QString DeviceChooser::generateDeviceText(const Solid::Device & volumeDevice)
{
    // guaranteed non-null
    auto * volume = volumeDevice.as<StorageVolume>();
    auto * block = volumeDevice.as<Block>();

    auto label = volume->label();
    auto vendor = volumeDevice.vendor();
    auto product = volumeDevice.product();

    if (vendor.trimmed().toLower() == product.trimmed().toLower()) {
        product.clear();
    }

    product = static_cast<QString>(vendor % " " % product).trimmed();
    auto hasLabel = !label.isEmpty();
    auto hasProduct = !product.isEmpty();

    if (hasLabel && hasProduct) {
        return QStringLiteral("%1 %2 (%3) [%4]")
                .arg(generateSizeText(volume->size()))
                .arg(product)
                .arg(label)
                .arg(block->device());
    }
    else if (hasLabel && !hasProduct) {
        return QStringLiteral("%1 Drive (%2) [%3]")
                .arg(generateSizeText(volume->size()))
                .arg(label)
                .arg(block->device());
    } else if (!hasLabel && hasProduct) {
        return QStringLiteral("%1 %2 [%3]")
                .arg(generateSizeText(volume->size()))
                .arg(product)
                .arg(block->device());
    } else {
        return tr("Unlabeled %1 Drive [%2]")
                .arg(generateSizeText(volume->size()))
                .arg(block->device());
    }
}

QString DeviceChooser::lastSelected() const
{
    return m_lastSelectedDevice;
}

void DeviceChooser::refreshDeviceList()
{
    m_ui->device->clear();
    m_ui->device->addItem(tr("Select device"));

    for (const auto & device : Device::listFromType(DeviceInterface::StorageDrive, {})) {
        if (!device.is<StorageDrive>()) {
            qDebug() << "Device" << device.vendor() << device.product() << "is not a storage device";
            continue;
        }

        auto * drive = device.as<StorageDrive>();

        if (!drive) {
            qWarning() << "Device" << device.vendor() << device.product() << "could not be used as a StorageDrive instance";
            continue;
        }

        if (!canUseDrive(*drive)) {
            continue;
        }

        const auto volumeDevice = partitionTableVolume(device);

        if (!volumeDevice) {
            continue;
        }

        int idx = m_ui->device->count();
        m_ui->device->addItem(generateDeviceText(*volumeDevice));
        m_ui->device->setItemData(idx, volumeDevice->as<Block>()->device(), DeviceFileRole);
    }

    m_ui->device->setCurrentText(lastSelected());
}

void DeviceChooser::updateLastSelected(int idx)
{
    // NOTE can be -1
    if (1 > idx) {
        return;
    }

    m_lastSelectedDevice = m_ui->device->currentText();
}
