// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "FinToast.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QFrame>
#include <QPropertyAnimation>
#include <QScreen>

#include <qgraphicseffect.h>

#include "FinTranslatorCore.h"

#include "Widgets/FinTranslatorMainWidget.h"


class FinToastWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int targetPos READ getTargetPos WRITE setTargetPos)
    Q_PROPERTY(float toastRatio READ getToastRatio WRITE setToastRatio)

public:
    explicit FinToastWidget(const QString& inMsg, QWidget* parent, const int inExpireTime);
    ~FinToastWidget() override;

private:
    int getTargetPos() const { return _targetPos; };
    void setTargetPos(const int inTargetPos) { _targetPos = inTargetPos; };

    float getToastRatio() const { return _toastRatio; };
    void setToastRatio(const float inToastRatio);;

    QVBoxLayout* _layout;
    QLabel* _label;

    QGraphicsOpacityEffect* _effect;

    QPropertyAnimation* _startAnim;
    QPropertyAnimation* _endAnim;
    QTimer _expireTimer;

    int _targetPos = 200; // 토스트 메시지를 띄울 위치. 부모 위젯에 상대 위치
    float _toastRatio;     // 시작, 종료 애니메이션에서 현재 단계를 비율로 나타냅니다.(투명도, 위치 등)
};

#include "FinToast.moc"

FinToastWidget::FinToastWidget(const QString& inMsg, QWidget* parent, const int inExpireTime)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TranslucentBackground);

    _layout = new QVBoxLayout(this);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    _label = new QLabel(this);
    _label->setText(inMsg);

    _layout->addWidget(_label);

    adjustSize();

    _effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(_effect);

    int _animDuration = 300;
    _startAnim = new QPropertyAnimation(this, "toastRatio", this);
    _startAnim->setDuration(_animDuration);
    _startAnim->setEasingCurve(QEasingCurve::OutCubic);
    _startAnim->setStartValue(0);
    _startAnim->setEndValue(1);

    _endAnim = new QPropertyAnimation(this, "toastRatio", this);
    _endAnim->setDuration(_animDuration);
    _endAnim->setEasingCurve(QEasingCurve::InCubic);
    _endAnim->setStartValue(1);
    _endAnim->setEndValue(0);

    _expireTimer.setInterval(inExpireTime);
    _expireTimer.setSingleShot(true);

    connect(_startAnim, &QAbstractAnimation::finished, this, [this]()
    {
        _expireTimer.start();
    });

    connect(&_expireTimer, &QTimer::timeout, this, [this]()
    {
        _endAnim->start();
        connect(_endAnim, &QAbstractAnimation::finished, this, [this]()
        {
            close();
            deleteLater();
        });
    });


    _startAnim->start();

    show();
}

FinToastWidget::~FinToastWidget() {
}

void FinToastWidget::setToastRatio(const float inToastRatio)
{
    _toastRatio = qBound(0.0f, inToastRatio, 1.0f);

    QPoint newPoint(10, 10);
    const int currentPosY = _targetPos * _toastRatio;

    const QWidget* parentW = parentWidget();

    // 부모 위젯이 show 상태면 중앙 상단에 배치. 그외엔 우하단
    if (parentW && (parentW->isHidden() == false))
    {

        const QRect mainGeo     = parentW->geometry();
        const QPoint mainCenter = QPoint((mainGeo.width() / 2) - (width() / 2), 0);

        newPoint = mainCenter + QPoint(0, currentPosY);
    }
    else if (const auto pScreen = qApp->primaryScreen())
    {
        const QPoint avBottomRight = pScreen->availableGeometry().bottomRight();

        newPoint = avBottomRight - (QPoint(width() + _targetPos, height() + currentPosY) /** 1.15*/);
    }

    move(newPoint);
    _effect->setOpacity(_toastRatio);
}

FinToast::FinToast(QObject* parent) : QObject(parent)
{
}

FinToast::~FinToast() {
}

// todo: 중복 메시지를 모아서 내보내는 기능 추가
// todo: 여러 메시지가 나올때 채팅창처럼 순차적으로 표기되도록 해야함.
// FinToastWidget map
void FinToast::showToast(const QString& inMessage, QWidget* inToastParent, const int inDuration)
{
    new FinToastWidget(inMessage, inToastParent, inDuration);
}
