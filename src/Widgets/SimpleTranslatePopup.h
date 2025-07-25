// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SIMPLETRANSLATEPOPUP_H
#define SIMPLETRANSLATEPOPUP_H

#include <QWidget>

#include "ITranslateWidget.h"

#include "FinTypes.h"

class LoadingBar;
class QPushButton;
class QSizeGrip;
class QScrollBar;
class QPropertyAnimation;
class FinTranslatorCore;
class QVBoxLayout;
class QLabel;

QT_BEGIN_NAMESPACE

namespace Ui
{
class SimpleTranslatePopup;
}

QT_END_NAMESPACE


class SimpleTranslatePopup : public ITranslateWidget
{
    Q_OBJECT

    Q_PROPERTY(QSize textEditSize READ getTextEditSize WRITE setTextEditSize)

public:
    explicit SimpleTranslatePopup(QWidget* parent = nullptr);

    ~SimpleTranslatePopup() override;

    virtual void completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle) override;

protected:
    virtual void applyTranslation(const QString& inTranslatedText, const TextStyle inTextStyle) override;

    virtual QScrollBar* getVerticalScrollBar() override;
    virtual QScrollBar* getHorizontalScrollBar() override;


    /** 입력된 문자열로 교체하고, 적정 사이즈로 팝업을 엽니다. */
    void showTranslationPopup(const QString& inTranslatedText, const TextStyle inTextStyle);

    void setMarkdown(const QString& inMarkdownStr);

    QSize getTextEditSize() const { return _textEditSize; };

    /** 텍스트 에디트 사이즈를 기반으로 전체 Widget의 크기와 위치를 계산 및 적용합니다. */
    void setTextEditSize(const QSize& inTextEditSize);

private:
    void manualSizeMode();

    void setupUI();

    /** 입력된 inNewText에 적합한 에디터의 크기를 계산합니다. */
    QSize calculateTextEditSize(const QString& inNewText) const;

    /** 입력된 사이즈를 목표로 애니메이션을 실행합니다. */
    void animateTextEditResize(const QSize& inNewSize);

    /** 애니메이션 종료 후, 적정 크기인지 확인합니다. */
    void adjustSizeAfterAnimationFinished();

    /** 수동 사이즈 계산에 필요한 text edit와 관련된 레이아웃 정보들을 계산합니다. */
    void calculateTextEditLayoutInfo();

    /**
     * 내부 스크롤바 값 변경 시, 외부 스크롤바에 이를 반영합니다.
     * Range, PageStep, Value 등을 동기화에 필요한 데이터를 적용합니다.
     */
    void syncInOutScrollbar();

public: signals:
    void abortTranslateReq();
    
protected:
    void onAlwaysOnToggle(bool checked);
    void onWindowModeToggle(bool checked);

    void changeNormalWindowMode();
    void changePopupMode();

    void setMaxNormal(const bool bMaximize);
    void onMaxNormalToggle(const bool bMaximize);

    void onMinimized();

    void setShadowEffectEnabled(const bool bIsEnable);

protected slots:
    void detectFocusInOut(QWidget* old, QWidget* now);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

    virtual bool eventFilter(QObject* obj, QEvent* event) override;

    QPropertyAnimation* _animation;

    FinWidgetModeFlags _widgetModeFlags; 


    QSize _textEditSize;

    QMargins _outMargins;

    QSize _innerMarginSize;
    QSize _outerMarginSize;

    QSize _minEditSize;
    QSize _maxEditSize;

    QSize _prevSize;

    int _lineCount = 0;
    int _lastLineLength = 0;

    // ~================
    const QSizeF _minSizeRatio  = {0.15f, 0.15f};
    const QSizeF _maxSizeRatio  = {0.2f, 0.65f};
    const QSizeF _fullSizeRatio = {0.95f, 0.95f};

    const QPointF _centerPosRatio = {0.85f, 0.33f};
    const float _yPosMaxRatio = 0.175f;

    bool _bManualSizeMode = false;

    bool _bMaximizedMode = false;

    bool _bIsDrag = false;
    QPoint _dragPoint;

    qreal _fontSize = 14.0f;

private:
    Ui::SimpleTranslatePopup* ui;

    QPushButton* _AlwaysOnButton;
    QPushButton* _windowModeButton;

    QPushButton* _maxRestoreButton;
    QPushButton* _minimizedButton;
    QPushButton* _closeButton;

    LoadingBar* _loadingBar;

    QSizeGrip* _sizeGrip;
};


#endif //SIMPLETRANSLATEPOPUP_H
