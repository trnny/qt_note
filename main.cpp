#include "vie.h"
#include <QApplication>
#include "stgc.h"
#include <QEvent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Vie w;

    return a.exec();
}
