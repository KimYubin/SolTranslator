#ifndef FINTRANSLATOR_H
#define FINTRANSLATOR_H

#include <QWidget>


class TranslateManager;
class GlobalHotKeyManager;
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

    void onSimpleTranslate(const QString& InOriginText);

private slots:
    void on_findButton_clicked();

private:
    void loadSettings();
    void loadAPI();
    
    Ui::FinTranslator* ui;

    TranslateManager* translateManager;
    GlobalHotKeyManager* globalHotKeyManager; 

    

};
#endif // FINTRANSLATOR_H
