// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLTOOLTIPBALLON_H
#define SOLTRANSLATOR_SOLTOOLTIPBALLON_H

#include <QPointer>
#include <QTimer>
#include <QWidget>

struct ToolTipData;
class QVBoxLayout;
class QLabel;

/** 커스텀 툴팁 말풍선 */
class SolToolTipBallon : public QWidget
{
    Q_OBJECT

    // qss에서 사용
    Q_PROPERTY(int triangleBaseWidth READ getTriangleBaseWidth WRITE setTriangleBaseWidth)
    Q_PROPERTY(int triangleHeight READ getTriangleHeight WRITE setTriangleHeight)
    Q_PROPERTY(int borderRadius READ getBorderRadius WRITE setBorderRadius)
    Q_PROPERTY(float borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)

    static QPointer<SolToolTipBallon> _ins;
    explicit SolToolTipBallon(QWidget* parent = nullptr);

public:
    static SolToolTipBallon* instance()
    {
        if (_ins.isNull())
        {
            _ins = new SolToolTipBallon();
        }
        return _ins;
    }

    void showToolTip(const QWidget* widget);
    void hideToolTipImmediately();

    /**
     * 툴팁이 보여지고 있다면, 새로운 툴팁으로 업데이트 합니다.
     */
    void updateWidgetToolTip(const QWidget* inWidget);

private:
    void showToolTipImpl(const QWidget* widget);
    void hideToolTipDelay();

protected:
    virtual void paintEvent(QPaintEvent*) override;

private:
    // property
    int    getTriangleBaseWidth() const;
    void   setTriangleBaseWidth(const int inWidth);
    int    getTriangleHeight() const;
    void   setTriangleHeight(const int inHeight);
    int    getBorderRadius() const;
    void   setBorderRadius(const int inRad);
    float  getBorderWidth() const;
    void   setBorderWidth(const float inWidth);
    int    getSpacing() const;
    void   setSpacing(const int inSpacing);
    QColor getBackgroundColor() const;
    void   setBackgroundColor(const QColor inColor);
    QColor getBorderColor() const;
    void   setBorderColor(const QColor inColor);

    void updateMargins() const;

    QPointer<const QWidget> _currentTargetWidget;

    QLabel* _label;
    QVBoxLayout* _layout;

    int _triangleBaseWidth;
    int _triangleHeight;
    int _borderRadius;
    qreal _borderWidth;

    // 위젯과의 간격
    int _spacing;

    QColor _backgroundColor;
    QColor _borderColor;

    QTimer _expireTimer;
    QTimer _hideTimer;

    enum class ShowDirection
    {
        Top
      , Right
      , Bottom
      , Left
    };

    QMargins triMargins(const ShowDirection inDirection) const;

    // 툴팁의 생성 위치. 타겟 위젯에서 바라보는 방향.
    ShowDirection _direction = ShowDirection::Top;
    QPoint triVertex;
};


#endif //SOLTRANSLATOR_SOLTOOLTIPBALLON_H