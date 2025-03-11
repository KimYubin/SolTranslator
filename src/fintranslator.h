#ifndef FINTRANSLATOR_H
#define FINTRANSLATOR_H

#include <QWidget>

#include "TranslateManager.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class FinTranslator;
}

QT_END_NAMESPACE

class FinTranslator : public QWidget
{
    Q_OBJECT

public:
    FinTranslator(QWidget* parent = nullptr);
    ~FinTranslator();

private slots:
    void on_findButton_clicked();

private:
    void loadTextFile();
    
    Ui::FinTranslator* ui;

    std::unique_ptr<TranslateManager> translateManager;

    

};
#endif // FINTRANSLATOR_H
