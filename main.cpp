#include "imageutility.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageUtility w;
    w.show();
    return a.exec();
}
