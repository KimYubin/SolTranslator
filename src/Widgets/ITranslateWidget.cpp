//
// Created by YubinKim on 25/06/01 일.
//

#include "ITranslateWidget.h"

#include <QScrollBar>
#include <QTimer>

ITranslateWidget::ITranslateWidget(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    _updateStreamStrTimer = new QTimer(this);
    _updateStreamStrTimer->setInterval(50);
    _updateStreamStrTimer->setSingleShot(true);
    connect(_updateStreamStrTimer, &QTimer::timeout, this, [this]()
    {
        setTranslationWithFixedScroll(_prevString, _prevTextStyle);
    });
}

ITranslateWidget::~ITranslateWidget()
{
}

void ITranslateWidget::streamTransText(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    _prevString    = inTranslatedText;
    _prevTextStyle = inTextStyle;
    _updateStreamStrTimer->start();
}

void ITranslateWidget::completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    _updateStreamStrTimer->stop();
    setTranslationWithFixedScroll(inTranslatedText, inTextStyle);
}

void ITranslateWidget::setTranslationWithFixedScroll(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    const int prevVerticalScrollVal   = getVerticalScrollBar()->value();
    const int prevHorizontalScrollVal = getHorizontalScrollBar()->value();

    applyTranslation(inTranslatedText, inTextStyle);

    getVerticalScrollBar()->setValue(prevVerticalScrollVal);
    getHorizontalScrollBar()->setValue(prevHorizontalScrollVal);
}
