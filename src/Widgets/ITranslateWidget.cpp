// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "ITranslateWidget.h"

#include <QScrollBar>
#include <QTimer>

#include "EngineUnits/TranslateUnit.h"

#include "SubWidgets/CustomMenuTextEdit.h"

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
    abortTrUnit();
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

void ITranslateWidget::abortTrUnit()
{
    if (_trUnit)
    {
        // todo: history 개발 후, 중단 대신 history에 기록하도록 해야합니다.
        _trUnit->abortTranslate();
    }
}

void ITranslateWidget::setTrUnit(TranslateUnit* inTrUnit)
{
    _trUnit = inTrUnit;
}

void ITranslateWidget::setTranslationWithFixedScroll(const QString& inTranslatedText, const TextStyle inTextStyle)
{
    const int prevVerticalScrollVal   = getVerticalScrollBar()->value();
    const int prevHorizontalScrollVal = getHorizontalScrollBar()->value();
    const int prevTextCursorPos       = getTextCursor().position();

    applyTranslation(inTranslatedText, inTextStyle);

    getVerticalScrollBar()->setValue(prevVerticalScrollVal);
    getHorizontalScrollBar()->setValue(prevHorizontalScrollVal);

    QTextCursor textCursor = getTextCursor();
    textCursor.setPosition(prevTextCursorPos);
    setTextCursor(textCursor);
}
