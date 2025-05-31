//
// Created by YubinKim on 25/06/01 일.
//

#include "ITranslateWidget.h"

#include <QTimer>

ITranslateWidget::ITranslateWidget(QWidget* parent, Qt::WindowFlags flags): QWidget(parent, flags)
{
    _updateStreamStrTimer = new QTimer(this);
    _updateStreamStrTimer->setInterval(50);
    _updateStreamStrTimer->setSingleShot(true);
    connect(_updateStreamStrTimer, &QTimer::timeout, this, [this]()
    {
        applyTranslation(_prevString, _prevTextStyle);
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
    applyTranslation(inTranslatedText, inTextStyle);
}
