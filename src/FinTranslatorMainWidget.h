//
// Created by YubinKim on 25/03/19 수.
//

#ifndef FINTRANSLATORMAINWIDGET_H
#define FINTRANSLATORMAINWIDGET_H

#include <QSystemTrayIcon>
#include <QWidget>


class QButtonGroup;
class SettingsWidget;
class TextEditTranslateWidget;
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

    // ~=================
    // theme
    void applyTheme();
    
    QString applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors);
    
protected:
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createTrayIcon();
    void setIcon();

    FinTranslatorCore* finCore;

    Ui::FinTranslatorMainWidget* ui;

    
    // ~==============
    // sub widgets
    QButtonGroup* _buttonGroup;
    TextEditTranslateWidget* textEditTranslate;
    SettingsWidget* settingsWidget;
    

    // ~==============
    // trayIcon
    QAction* miniToTrayAction;
    QAction* restoreAction;
    QAction* quitAction;

    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;
};


#endif //FINTRANSLATORMAINWIDGET_H
