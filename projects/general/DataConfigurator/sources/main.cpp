#include "Qt/DataConfigurator.h"
#include <QtWidgets/QApplication>
#include "Common.h"

int main(int argc, char *argv[])
{
    LOG_INTT("logs/log.txt");
    LOG_DEBUG("initializing QApplication");
    QApplication a(argc, argv);

    LOG_DEBUG("initializing DataConfigurator");
    DataConfigurator w;

    LOG_DEBUG("showing DataConfigurator");
    w.show();

    LOG_DEBUG("executing QApplication");
    int result = a.exec();

    LOG_DEBUG("log finished");
    return result;
}
