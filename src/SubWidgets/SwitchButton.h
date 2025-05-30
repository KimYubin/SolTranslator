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

public:
    explicit SwitchButton(QWidget* parent = nullptr);

    virtual QSize sizeHint() const override;
    virtual bool hitButton(const QPoint& pos) const override;

    /**
     * 
     * @param inSize button size
     * @param inTrackHeightRatio size 높이 대비 트랙의 높이 비율 
     * @param inHandleRatio 트랙 높이 대비 핸들 지름의 비율
     */
    void setButtonShape(const QSize& inSize, const float inTrackHeightRatio, const float inHandleRatio);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

private slots:
    void setupAnimation(const Qt::CheckState inCheckState);

private:
    float handlePosition() const;
    void setHandlePosition(const float position);

    QBrush _barBrush;
    QBrush _barCheckedBrush;
    QBrush _handleBrush;
    QBrush _handleCheckedBrush;

    QPen _transparentPen;
    QPen _lightGreyPen;

    // size
    QSize _size;             // sizeHint
    float _trackHeightRatio; // 콘텐츠 사이즈 대비 트랙 높이 비율
    float _handleRadRatio;   // 트랙 높이 대비 핸들 반지름 비율

    // animation
    QSequentialAnimationGroup* _animationGroup;
    QPropertyAnimation* _handleAnimation;

    float _handlePos = 0.0f;
};


#endif //SWITCHBUTTON_H
