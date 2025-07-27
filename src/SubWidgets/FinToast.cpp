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
    Q_PROPERTY(int showPos READ getshowPos WRITE setshowPos)
    Q_PROPERTY(int popPos READ getpopPos WRITE setpopPos)
    Q_PROPERTY(float toastRatio READ getToastRatio WRITE setToastRatio)

public:
    explicit FinToastWidget(const QString& inMsg
                          , QWidget* targetWidget
                          , QWidget* parent
                          , const int inExpireTime);
    ~FinToastWidget() override;

private:
    int getshowPos() const { return _showPos; };
    void setshowPos(const int inshowPos) { _showPos = inshowPos; };
    int getpopPos() const { return _popPos; };
    void setpopPos(const int inpopPos) { _popPos = inpopPos; };

    float getToastRatio() const { return _toastRatio; };
    void setToastRatio(const float inToastRatio);;

    QPointer<QWidget> _targetWidget;

    QVBoxLayout* _layout;
    QLabel* _label;

    QGraphicsOpacityEffect* _effect;

    QPropertyAnimation* _startAnim;
    QPropertyAnimation* _endAnim;
    QTimer _expireTimer;

    int _showPos = 15; // show toast 상대 위치.
    int _popPos  = 5;  // pop toast 상대 위치

    float _toastRatio; // 시작, 종료 애니메이션에서 현재 단계를 비율로 나타냅니다.(투명도, 위치 등)
};

#include "FinToast.moc"

FinToastWidget::FinToastWidget(const QString& inMsg
                             , QWidget* targetWidget
                             , QWidget* parent
                             , const int inExpireTime)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint)
    , _targetWidget(targetWidget)
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
    if (_targetWidget.isNull())
    {
        _endAnim->setEndValue(0);
    }
    else
    {
        _endAnim->setEndValue(2);
    }

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
    _toastRatio = qBound(0.0f, inToastRatio, 2.0f);

    // 위치 조정
    QPoint newPoint(10, 10);

    if (parentWidget() && (parentWidget()->isHidden() == false))
    {
        // show toast: 부모 위젯의 내부 중앙 상단에 배치
        const int currentPosY   = _showPos * _toastRatio;
        const QRect mainGeo     = parentWidget()->geometry();
        const QPoint mainCenter = QPoint((mainGeo.width() / 2) - (width() / 2), 0);

        newPoint = mainCenter + QPoint(0, currentPosY);
    }
    else if (_targetWidget && (_targetWidget->isHidden() == false))
    {
        // pop toast: 대상 위젯의 바깥쪽 중앙 위에 배치
        const int currentPosY  = _popPos * _toastRatio;
        const QRect mainGeo    = _targetWidget->geometry();
        const QPoint centerTop = QPoint((mainGeo.width() / 2) - (width() / 2)
                                      , -(height() + currentPosY));

        newPoint = _targetWidget->mapToGlobal(centerTop);
    }
    else if (const auto pScreen = qApp->primaryScreen())
    {
        // show, pop 대상 위젯이 보이지 않을 때, 화면 우측 하단에 배치
        const int currentPosY   = _showPos * _toastRatio;
        const QPoint avScreenBR = pScreen->availableGeometry().bottomRight();

        newPoint = avScreenBR - (QPoint(width() + _showPos, height() + currentPosY));
    }

    move(newPoint);

    // 투명도 조정
    const float toastOpacity = _toastRatio > 1.0f ? (2.0f - _toastRatio) : _toastRatio;
    _effect->setOpacity(toastOpacity);
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
    new FinToastWidget(inMessage, nullptr, inToastParent, inDuration);
}

void FinToast::popToastOnWidget(const QString& inMessage, QWidget* inTargetWidget, const int inDuration)
{
    new FinToastWidget(inMessage, inTargetWidget, nullptr, inDuration);
}
