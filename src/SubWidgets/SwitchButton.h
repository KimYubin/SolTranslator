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

    // qss에서 사용
    Q_PROPERTY(QSize switchSize READ getSwitchSize WRITE setSwitchSize)
    Q_PROPERTY(float trackHeightRatio READ getTrackHeightRatio WRITE setTrackHeightRatio)
    Q_PROPERTY(float HandleRatio READ getHandleRatio WRITE setHandleRatio)

    Q_PROPERTY(QColor barCheckedColor READ getBarCheckedColor WRITE setBarCheckedColor)
    Q_PROPERTY(QColor barUncheckedColor READ getBarUncheckedColor WRITE setBarUncheckedColor)
    Q_PROPERTY(QColor handleCheckedColor READ getHandleCheckedColor WRITE setHandleCheckedColor)
    Q_PROPERTY(QColor handleUncheckedColor READ getHandleUncheckedColor WRITE setHandleUncheckedColor)
    Q_PROPERTY(QColor barPenColor READ getBarPenColor WRITE setBarPenColor)
    Q_PROPERTY(QColor handleCheckedPenColor READ getHandleCheckedPenColor WRITE setHandleCheckedPenColor)
    Q_PROPERTY(QColor handleUncheckedPenColor READ getHandleUncheckedPenColor WRITE setHandleUncheckedPenColor)

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

    QSize getSwitchSize() const;
    void setSwitchSize(const QSize& inSize);

    float getTrackHeightRatio() const;
    void setTrackHeightRatio(const float inRatio);

    float getHandleRatio() const;
    void setHandleRatio(const float inRatio);

    QColor getBarCheckedColor() const;
    void setBarCheckedColor(const QColor& inColor);
    QColor getBarUncheckedColor() const;
    void setBarUncheckedColor(const QColor& inColor);
    QColor getHandleCheckedColor() const;
    void setHandleCheckedColor(const QColor& inColor);
    QColor getHandleUncheckedColor() const;
    void setHandleUncheckedColor(const QColor& inColor);
    QColor getBarPenColor() const;
    void setBarPenColor(const QColor& inColor);
    QColor getHandleCheckedPenColor() const;
    void setHandleCheckedPenColor(const QColor& inColor);
    QColor getHandleUncheckedPenColor() const;
    void setHandleUncheckedPenColor(const QColor& inColor);
    

    QBrush _barCheckedBrush;
    QBrush _barUncheckedBrush;
    QBrush _handleCheckedBrush;
    QBrush _handleUncheckedBrush;

    QPen _barPen;
    QPen _handleCheckedPen;
    QPen _handleUncheckedPen;

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
