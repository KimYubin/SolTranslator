// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINTRANSLATORMAINWIDGET_H
#define FINTRANSLATORMAINWIDGET_H

#include <QPointer>
#include <QSystemTrayIcon>
#include <QWidget>

#include "IFinWidget.h"


class QComboBox;
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
    static void applyTheme(const QString& inThemeName = "dark");

    QString applyThemeColor(const QString& templateTheme, const std::unordered_map<QString, QString>& colors);

protected:
    virtual void closeEvent(QCloseEvent* event) override;

protected slots:
    /** 앱을 종료합니다. */
    void quitApp();

    /**
     * 앱 종료 이벤트로 발생하는 신호를 받습니다.
     * @see QCoreApplication::aboutToQuit
     */
    void onAppQuitEvent() const;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void createActions();
    void createTrayIcon();
    void popupTrayMenu();

    Ui::FinTranslatorMainWidget* ui;

    QComboBox* _engineSelector;

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

public:
    static void updatePaletteColor();

private:
    // PaletteColor
    Q_PROPERTY(QColor windowColor READ getWindowColor WRITE setWindowColor)
    Q_PROPERTY(QColor windowTextColor READ getWindowTextColor WRITE setWindowTextColor)
    Q_PROPERTY(QColor baseColor READ getBaseColor WRITE setBaseColor)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QColor buttonColor READ getButtonColor WRITE setButtonColor)
    Q_PROPERTY(QColor buttonTextColor READ getButtonTextColor WRITE setButtonTextColor)
    Q_PROPERTY(QColor highlightColor READ getHighlightColor WRITE setHighlightColor)
    Q_PROPERTY(QColor highlightedTextColor READ getHighlightedTextColor WRITE setHighlightedTextColor)
    Q_PROPERTY(QColor linkColor READ getLinkColor WRITE setLinkColor)
    Q_PROPERTY(QColor disableColor READ getDisableColor WRITE setDisableColor)

    void setWindowColor(const QColor& inColor);
    void setWindowTextColor(const QColor& inColor);
    void setBaseColor(const QColor& inColor);
    void setTextColor(const QColor& inColor);
    void setButtonColor(const QColor& inColor);
    void setButtonTextColor(const QColor& inColor);
    void setHighlightColor(const QColor& inColor);
    void setHighlightedTextColor(const QColor& inColor);
    void setLinkColor(const QColor& inColor);
    void setDisableColor(const QColor& inColor);

    QColor getWindowColor() const;
    QColor getWindowTextColor() const;
    QColor getBaseColor() const;
    QColor getTextColor() const;
    QColor getButtonColor() const;
    QColor getButtonTextColor() const;
    QColor getHighlightColor() const;
    QColor getHighlightedTextColor() const;
    QColor getLinkColor() const;
    QColor getDisableColor() const;

    static QColor _windowColor;
    static QColor _windowTextColor;
    static QColor _baseColor;
    static QColor _textColor;
    static QColor _buttonColor;
    static QColor _buttonTextColor;
    static QColor _highlightColor;
    static QColor _highlightedTextColor;
    static QColor _linkColor;
    static QColor _disableColor;
};


#endif //FINTRANSLATORMAINWIDGET_H
