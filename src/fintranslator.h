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

    virtual void setVisible(bool visible) override;

protected:
    virtual void closeEvent(QCloseEvent* event) override;

public:
    void onSimpleTranslate(const QString& InOriginText);

private slots:
    void on_findButton_clicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void loadSettings();
    void loadAPI();

    void createActions();
    void createTrayIcon();
    void setIcon();


    Ui::FinTranslator* ui;

    TranslateManager* translateManager;
    GlobalHotKeyManager* globalHotKeyManager;

    // ~==============
    // trayIcon
    QAction* miniToTrayAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};

#endif // FINTRANSLATOR_H
