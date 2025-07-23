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


class FinToastWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(int toastPosY READ getToastPosY WRITE setToastPosY)
    Q_PROPERTY(float toastRatio READ getToastRatio WRITE setToastRatio)

public:
    explicit FinToastWidget(const QString& inMsg, const int inExpireTime, QWidget* parent = nullptr);
    ~FinToastWidget() override;

private:
    int getToastPosY() const { return _toastPosY; };
    void setToastPosY(const int inToastPosY) { _toastPosY = inToastPosY; };

    float getToastRatio() const { return _toastRatio; };
    void setToastRatio(const float inToastRatio);;

    QLabel* _label;
    QVBoxLayout* _layout;

    QGraphicsOpacityEffect* _effect;

    QPropertyAnimation* _startAnim;
    QPropertyAnimation* _endAnim;
    QTimer _expireTimer;

    int _toastPosY = 200;
    float _toastRatio; // 시작, 종료 애니메이션 현재 단계를 비율로 나타냅니다.(투명도, 위치 등)
};

#include "FinToast.moc"

FinToastWidget::FinToastWidget(const QString& inMsg, const int inExpireTime, QWidget* parent)
    : QFrame(parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_ShowWithoutActivating);

    _label = new QLabel(this);
    _label->setText(inMsg);
    _layout = new QVBoxLayout(this);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);
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

    // 추후 종료시간 초기화 혹은 연장 기능을 위해, singleShot 대신 멤버 변수를 사용합니다. 
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

    FinTranslatorMainWidget* _mainWidget = finCore->getFinMainWidget();
    if (_mainWidget && (_mainWidget->isHidden() == false))
    {
        setParent(_mainWidget);

        const int targetPosY = _toastPosY * _toastRatio;

        const QRect mainGeo     = _mainWidget->geometry();
        const QPoint mainCenter = QPoint((mainGeo.width() / 2) - (width() / 2), 0);
        const QPoint newPoint   = mainCenter + QPoint(0, targetPosY);

        move(newPoint);
    }
    else if (const auto pScreen = qApp->primaryScreen())
    {
        const QRect avGeo     = pScreen->availableGeometry();
        const QPoint bR       = avGeo.bottomRight();
        const QPoint newPoint = bR - QPoint((width() * 1.15), (height() * 1.15));

        move(newPoint);
    }

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
void FinToast::showToast(const QString& inMessage, const int inDuration)
{
    new FinToastWidget(inMessage, inDuration);
}
