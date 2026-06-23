#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    // QApplication must be constructed before any widget. It manages
    // the event loop, application-wide settings, and the OS event queue.
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    // app.exec() blocks here, running the event loop until the user
    // closes the window — this is the heart of every Qt GUI app.
    return app.exec();
}