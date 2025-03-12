#include "fintranslator.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    FinTranslator w;
    w.show();

    return app.exec();
}
