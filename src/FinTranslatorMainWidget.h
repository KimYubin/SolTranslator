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

    FinTranslatorCore* _finCore;

    Ui::FinTranslatorMainWidget* ui;

    
    // ~==============
    // sub widgets
    QButtonGroup* _buttonGroup;
    TextEditTranslateWidget* _textEditTranslate;
    SettingsWidget* _settingsWidget;
    

    // ~==============
    // trayIcon
    QAction* _miniToTrayAction;
    QAction* _restoreAction;
    QAction* _quitAction;

    QSystemTrayIcon* _trayIcon;
    QMenu* _trayIconMenu;
};


#endif //FINTRANSLATORMAINWIDGET_H
