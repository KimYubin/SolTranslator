//
// Created by YubinKim on 25/05/27 화.
//

#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QCheckBox>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QBrush>
#include <QPen>


class SwitchButton : public QCheckBox
{
    Q_OBJECT

    Q_PROPERTY(float handlePosition READ handlePosition WRITE setHandlePosition)
    Q_PROPERTY(float pulseRadius READ pulseRadius WRITE setPulseRadius)

public:
    explicit SwitchButton(QWidget* parent = nullptr);

    virtual QSize sizeHint() const override;
    virtual bool hitButton(const QPoint& pos) const override;

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private slots:
    void setupAnimation(const Qt::CheckState inCheckState);

private:
    float handlePosition() const;
    void setHandlePosition(const float position);

    float pulseRadius() const;
    void setPulseRadius(const float radius);

    QBrush _barBrush;
    QBrush _barCheckedBrush;
    QBrush _handleBrush;
    QBrush _handleCheckedBrush;
    QBrush _pulseUncheckedBrush;
    QBrush _pulseCheckedBrush;

    QPen _transparentPen;
    QPen _lightGreyPen;

    // size
    QSize _size;             // sizeHint
    float _handleRadRatio;   // 콘텐츠 사이즈 대비 핸들 반지름 비율
    float _trackHeightRatio; // 콘텐츠 사이즈 대비 트랙 높이 비율

    // animation
    QSequentialAnimationGroup* _animationGroup;
    QPropertyAnimation* _handleAnimation;
    QPropertyAnimation* _pulseAnimation;
    
    float _handlePos = 0.0f;
    float _pulseRad  = 0.0f;

};


#endif //SWITCHBUTTON_H
