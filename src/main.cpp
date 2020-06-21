#include <QApplication>

#include "mainwindow.h"

using ReScribe::MainWindow;

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    app.setApplicationDisplayName(QStringLiteral("ReScribe"));
    app.setApplicationName(QStringLiteral("ReScribe"));
    app.setApplicationVersion(QStringLiteral("0.1"));
    app.setOrganizationName(QStringLiteral("Équit"));
    app.setOrganizationDomain(QStringLiteral("org.equit"));
    MainWindow win;
    win.show();
    return app.exec();
}
