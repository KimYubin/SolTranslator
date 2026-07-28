// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef POPUPTRANSLATEWIDGET_H
#define POPUPTRANSLATEWIDGET_H

#include "ITranslateWidget.h"
#include "Types/SolTypes.h"

class ILoadingWidget;
class SolButton;
class LoadingBar;
class QPushButton;
class QSizeGrip;
class QScrollBar;
class QPropertyAnimation;
class SolTranslatorCore;
class QVBoxLayout;
class QLabel;

QT_BEGIN_NAMESPACE

namespace Ui
{
class PopupTranslateWidget;
}

QT_END_NAMESPACE


class PopupTranslateWidget : public ITranslateWidget
{
    Q_OBJECT

    Q_PROPERTY(QSize textEditSize READ getTextEditSize WRITE setTextEditSize)

public:
    explicit PopupTranslateWidget(QWidget* parent = nullptr);
    ~PopupTranslateWidget() override;

    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUI();

public:
    void executeTranslate(const QString& inSourceText
                        , const TextStyle inTextStyle
                        , const LangType inSourceLang
                        , const LangType inTargetLang
                        , const bool inIsIgnoreCache);


    virtual void completeTranslateText(const QString& inTargetText) override;

    void viewTranslationText(const QString& inSourceText
                           , const QString& inTargetText
                           , const TextStyle inTextStyle);

protected:
    virtual void applyTranslation() override;

    virtual QScrollBar* getVerticalScrollBar() const override;
    virtual QScrollBar* getHorizontalScrollBar() const override;
    virtual QTextCursor getTextCursor() const override;
    virtual void setTextCursor(const QTextCursor& cursor) override;


    /** 입력된 문자열로 교체하고, 적정 사이즈로 팝업을 엽니다. */
    void showTranslationPopup();

    QSize getTextEditSize() const { return _textEditSize; }

    /** 텍스트 에디트 사이즈를 기반으로 전체 Widget의 크기와 위치를 계산 및 적용합니다. */
    void setTextEditSize(const QSize& inTextEditSize);

private:
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

    void manualSizeMode();

protected:
    void onAlwaysOnToggle(const bool inChecked);
    void onWindowModeToggle(const bool inChecked);

    void changeNormalWindowMode();
    void changePopupMode();

    void setMaxNormal(const bool inIsMaximize);
    void onMaxNormalToggle(const bool inIsMaximize);

    void onMinimized();

    void toggleTranslationText();

    void setShadowEffectEnabled(const bool inIsEnable);

protected slots:
    void detectFocusInOut(const QWidget* old, const QWidget* now);

private:
    /**
     * 크기 조절용 안쪽 지오메트리 입니다.
     * 이 사각형보다 외곽은 크기 조절용 지역이고, 안쪽은 이동 구역입니다.
     * @return 
     */
    QRect getInnerGeometry() const;
    void moveWindow(const QPoint& inMousePos);
    void resizeWindow(const QPoint& inMousePos);
    void setCursorShape(const QPoint& inMousePos);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

protected:
    QPropertyAnimation* _animation;

    SolWidgetModeFlags _widgetModeFlags;


    QSize _textEditSize;

    QMargins _outMargins;

    QSize _innerMarginSize;
    QSize _outerMarginSize;

    QSize _minEditSize;
    QSize _maxEditSize;

    QSize _prevSize;

    // ~================
    const QSizeF _minSizeRatio;
    const QSizeF _maxSizeRatio; // 팝업창이 자동으로 커지는 한계 크기
    const QSizeF _fullSizeRatio;

    /**
     * 팝업창 중심 지점 위치.
     * 이 값을 중심으로 커지지만, _yPosMaxRatio을 넘겨서 커지지는 않습니다.
     */
    const QPointF _centerPosRatio;

    /**
     * 팝업창 자동 확장시, 창 좌상단의 최대 y축 위치.
     * 자동으로 커져도 이 값을 넘겨서 위로 올라가지는 않습니다. 
     */
    const qreal _yPosMaxRatio;

    bool _isManualSizeMode = false;

    bool _isMaximizedMode = false;

    bool _isDrag = false;
    QPoint _dragPoint;


    bool _isTranslateComplete = false;

    TextCategory _currentTextCategory = TextCategory::TargetText;

private:
    Ui::PopupTranslateWidget* ui;

    SolButton* _AlwaysOnButton;
    SolButton* _windowModeButton;

    SolButton* _maxRestoreButton;
    SolButton* _minimizedButton;
    SolButton* _closeButton;

    ILoadingWidget* _loadingWidget;


    SolButton* _reTranslateButton;
    SolButton* _textToggleButton;
    QSizeGrip* _sizeGrip;
};


#endif //POPUPTRANSLATEWIDGET_H
