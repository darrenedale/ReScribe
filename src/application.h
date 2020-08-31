//
// Created by darren on 27/06/2020.
//

#ifndef RESCRIBE_APPLICATION_H
#define RESCRIBE_APPLICATION_H

#include <memory>
#include <QApplication>
#include <QtDBus/QDBusInterface>
#include <QSystemTrayIcon>
#include "settings.h"

namespace ReScribe
{
    class MainWindow;

    class Application : public QApplication
    {
        Q_OBJECT
    public:
        static const constexpr int DefaultNotificationTimeout = 10000;

        Application(int & argc, char ** argv);
        Application(const Application &) = delete;
        Application(Application &&) = delete;
        void operator=(const Application &) = delete;
        void operator=(Application &&) = delete;
        ~Application() override;

        static Application * instance() {
            return dynamic_cast<Application *>(QApplication::instance());
        }

        MainWindow * mainWindow() const {
            return m_mainWindow.get();
        }

        void showNotification(const QString & message, const QString & title, int timeout = DefaultNotificationTimeout);
        void showNotification(const QString & message, int timeout = DefaultNotificationTimeout) {
            showNotification(message, QStringLiteral(), timeout);
        }

        const Settings & settings() const
        {
            return m_settings;

        }

        Settings & settings()
        {
            return m_settings;
        }

        static int exec();

    private:
        std::unique_ptr<MainWindow> m_mainWindow;
        QDBusInterface m_notificationsInterface;
        QSystemTrayIcon m_trayIcon;
        Settings m_settings;
    };
}

#define rsApp (Application::instance())

#endif //RESCRIBE_APPLICATION_H
