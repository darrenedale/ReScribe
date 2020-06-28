#include <QApplication>

#include "application.h"

using ReScribe::MainWindow;

int main(int argc, char ** argv)
{
    ReScribe::Application app(argc, argv);
    return app.exec();
}
