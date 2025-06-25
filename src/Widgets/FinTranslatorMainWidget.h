//
// Created by YubinKim on 25/03/19 수.
//

#ifndef FINTRANSLATORMAINWIDGET_H
#define FINTRANSLATORMAINWIDGET_H

#include <QPointer>
#include <QSystemTrayIcon>
#include <QWidget>

#include "IFinWidget.h"


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

class FinTranslatorMainWidget : public IFinWidget
{
    Q_OBJECT

public:
    explicit FinTranslatorMainWidget(QWidget* parent = nullptr);
    ~FinTranslatorMainWidget() override;

    virtual void setVisible(bool visible) override;

    void showSettingsWidget();

    // ~=================
    // theme
    void applyTheme(const QString& inThemeName = "dark");
    
    QString applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors);
    
protected:
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createTrayIcon();
    void popupTrayMenu();

    Ui::FinTranslatorMainWidget* ui;

    // ~==============
    // sub widgets
    QButtonGroup* _buttonGroup;
    TextEditTranslateWidget* _textEditTranslate;
    QPointer<SettingsWidget> _settingsWidget;
    

    // ~==============
    // trayIcon
    QAction* _miniToTrayAction;
    QAction* _restoreAction;
    QAction* _settingAction;
    QAction* _quitAction;

    QIcon _finIcon;
    QSystemTrayIcon* _trayIcon;
    QMenu* _trayIconMenu;

    // 좌클릭과 더블클릭 구분용
    QTimer* _doubleClickTimer;
    QPoint _prevMousePos;
};


#endif //FINTRANSLATORMAINWIDGET_H
