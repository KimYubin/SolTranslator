#include "fintranslator.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    FinTranslator w;
    w.show();
    return a.exec();
}
