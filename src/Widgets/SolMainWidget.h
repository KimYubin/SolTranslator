// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLMAINWIDGET_H
#define SOLMAINWIDGET_H

#include "ISolWidget.h"

#include <QPointer>
#include <QSystemTrayIcon>


class SolTrayIcon;
class SolButton;
class EngineSelector;
class QPushButton;
class QComboBox;
class QButtonGroup;
class SettingsWidget;
class TextEditTranslateWidget;
class SolTranslatorCore;
class GlobalHotKeyManager;
class TranslateManager;

QT_BEGIN_NAMESPACE

namespace Ui
{
class SolMainWidget;
}

QT_END_NAMESPACE

class SolMainWidget : public ISolWidget
{
    Q_OBJECT

public:
    explicit SolMainWidget(QWidget* parent = nullptr);
    ~SolMainWidget() override;

    void setVisible(const bool visible) override;

    void showSettings();

protected:
    void closeEvent(QCloseEvent* event) override;

protected slots:
    /** 앱을 종료합니다. */
    void quitApp();

    /**
     * 앱 종료 이벤트로 발생하는 신호를 받습니다.
     * @see QCoreApplication::aboutToQuit
     */
    void onAppQuitEvent();

private slots:
    void iconActivated(const QSystemTrayIcon::ActivationReason inReason);

signals:
    void visibleChanged(const bool inIsVisible);

private:
    void setupTrayIcon();
    void setupShortcuts();
    void popupTrayMenu();

    enum class TabMovement { Previous, Next };
    void moveTab(const TabMovement inMovement);

    Ui::SolMainWidget* ui;

    EngineSelector* _engineSelector;

    // ~==============
    // sub widgets
    QButtonGroup* _buttonGroup;
    SolButton* _textButton;
    SolButton* _docButton;
    SolButton* _historyButton;

    TextEditTranslateWidget* _textEditTranslate;
    QPointer<SettingsWidget> _settingsWidget;

    // ~==============
    // trayIcon
    QIcon _solIcon;
    SolTrayIcon* _trayIcon;

    QTimer* _doubleClickTimer;
    QPoint _prevMousePos;

};


#endif //SOLMAINWIDGET_H
