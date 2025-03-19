//
// Created by YubinKim on 25/03/19 수.
//

#ifndef FINTRANSLATORMAINWIDGET_H
#define FINTRANSLATORMAINWIDGET_H

#include <QSystemTrayIcon>
#include <QWidget>


class FinTranslatorCore;
class GlobalHotKeyManager;
class TranslateManager;
class DataManager;
QT_BEGIN_NAMESPACE

namespace Ui
{
class FinTranslatorMainWidget;
}

QT_END_NAMESPACE

class FinTranslatorMainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit FinTranslatorMainWidget(FinTranslatorCore* inFinCore, QWidget* parent = nullptr);
    ~FinTranslatorMainWidget() override;

    virtual void setVisible(bool visible) override;

protected:
    virtual void closeEvent(QCloseEvent* event) override;



private slots:
    void on_findButton_clicked();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void loadSettings();
    void loadAPI();

    void createActions();
    void createTrayIcon();
    void setIcon();

    FinTranslatorCore* finCore;

    Ui::FinTranslatorMainWidget* ui;


    // ~==============
    // trayIcon
    QAction* miniToTrayAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};


#endif //FINTRANSLATORMAINWIDGET_H
