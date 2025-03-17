#ifndef FINTRANSLATOR_H
#define FINTRANSLATOR_H

#include <QWidget>
#include <QSystemTrayIcon>


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


public:
    virtual void setVisible(bool visible) override;

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createTrayIcon();
    void setIcon();

    QAction* miniToTrayAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};

#endif // FINTRANSLATOR_H
