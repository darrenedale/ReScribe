#include <QApplication>

#include "application.h"

using ReScribe::MainWindow;

int main(int argc, char ** argv)
{
    ReScribe::Application app(argc, argv);
    app.setApplicationDisplayName(QStringLiteral("ReScribe"));
    app.setApplicationName(QStringLiteral("ReScribe"));
    app.setApplicationVersion(QStringLiteral("0.1"));
    app.setOrganizationName(QStringLiteral("Équit"));
    app.setOrganizationDomain(QStringLiteral("org.equit"));
    return app.exec();
}
