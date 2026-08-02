// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLSMOOTHSCROLLBAR_H
#define SOLTRANSLATOR_SOLSMOOTHSCROLLBAR_H

#include <QBasicTimer>
#include <QScrollBar>

class SolSmoothComponent;

/**
 * The SolSmoothScrollBar class is a QScrollBar-derived class
 * that provides smooth wheel scrolling.
 */
class SolSmoothScrollBar : public QScrollBar
{
    Q_OBJECT

public:
    explicit SolSmoothScrollBar(QWidget* inParent = nullptr);

    bool scrollSmoothToTargetValue(const int inTargetVal);
    bool scrollSmoothToDeltaValue(const int inDeltaVal);

    bool scrollSmoothToDeltaAngle(const float inDeltaAngle);
    bool isHorizontal() const;

    void setRepeatDelay(const int inRepeatDelay);
    void setRepeatDuration(const int inRepeatDuration);
    void setPageStepRepeatLimit(const int inPageStepRepeatLimit);

    void onActionTriggered(const int inAction);

    int pixelPosToRangeValue(const int inPos) const;

protected:
    void setRepeatTimer(const SliderAction inAction);
    void wheelEvent(QWheelEvent* inEvent) override;
    void mousePressEvent(QMouseEvent* inEvent) override;
    void mouseReleaseEvent(QMouseEvent* inEvent) override;
    void timerEvent(QTimerEvent* inEvent) override;

private:
    void stopRepeat();

protected:
    SolSmoothComponent* _smoothComponent;

    SliderAction _repeatAction = SliderNoAction;

    QBasicTimer _repeatTimer;

    int _repeatDelay;// 최초 반복 딜레이
    int _repeatDuration; // 이후 반복 간 간격
    int _pageStepRepeatLimit; // Move directly to the target point after the Repeat limit.

    int _repeatStack     = 0;
    int _pressRangeValue = 0;
    bool _isFirstAction  = false;
};



#endif //SOLTRANSLATOR_SOLSMOOTHSCROLLBAR_H
