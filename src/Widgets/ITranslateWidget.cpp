// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "ITranslateWidget.h"

#include <QScrollBar>
#include <QTimer>

ITranslateWidget::ITranslateWidget(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    _streamUpdateTimer = new QTimer(this);
    _streamUpdateTimer->setInterval(50);
    _streamUpdateTimer->setSingleShot(true);
    connect(_streamUpdateTimer, &QTimer::timeout, this, [this]()
    {
        setTranslationWithFixedScroll(_translatedText, _translatedTextStyle);
    });
}

ITranslateWidget::~ITranslateWidget()
{
}

void ITranslateWidget::streamTransText(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    _translatedText      = inTranslatedText;
    _translatedTextStyle = inTextStyle;
    _streamUpdateTimer->start();
}

void ITranslateWidget::completeTransText(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    _translatedText      = inTranslatedText;
    _translatedTextStyle = inTextStyle;
    _streamUpdateTimer->stop();
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
