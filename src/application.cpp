//
// Created by darren on 27/06/2020.
//

#include <QMessageBox>
#include <QDBusPendingReply>
#include <QIcon>
#include "application.h"
#include "mainwindow.h"

using namespace ReScribe;

Application::Application(int & argc, char ** argv)
: QApplication(argc, argv),
  m_mainWindow(std::make_unique<MainWindow>()),
  m_trayIcon(QIcon::fromTheme(QStringLiteral("rescribe"), QIcon(QStringLiteral(":/icons/systray")))),
  m_notificationsInterface(QStringLiteral("org.freedesktop.Notifications"),
     QStringLiteral("/org/freedesktop/Notifications"),
     QStringLiteral("org.freedesktop.Notifications"))
{
    m_trayIcon.show();
}

Application::~Application() = default;

void Application::showNotification(const QString & message, const QString & title, int timeout)
{
    if (m_notificationsInterface.isValid()) {
         auto reply = m_notificationsInterface.asyncCall(
            QLatin1String("Notify"),
            applicationDisplayName(),                   // app_name
            0,                                          // replaces_id
            QLatin1String("rescribe"),                  // app_icon
            title,                                      // summary
            message,                                    // body
            QStringList(),                              // actions
            QVariantMap(),                              // hints
            timeout                                     // timeout
        );

        if (reply.isValid()) {
            return;
        }
    }

    // TODO consider adding a tray icon
    if (QSystemTrayIcon::supportsMessages()) {
        m_trayIcon.showMessage(title, message, QSystemTrayIcon::Information, timeout);
    } else {
        QMessageBox::information(m_mainWindow.get(), title, message, QMessageBox::StandardButtons(QMessageBox::Ok));
    }
}

int Application::exec()
{
    rsApp->mainWindow()->show();
    return QApplication::exec();
}
