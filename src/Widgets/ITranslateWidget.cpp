// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#include "ITranslateWidget.h"

#include "EngineUnits/TranslateUnit.h"
#include "SubWidgets/CustomMenuTextEdit.h"

#include <QScrollBar>
#include <QTimer>

ITranslateWidget::ITranslateWidget(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , _translatedText("")
    , _translatedTextStyle(TextStyle::PlainText)
{
    _streamUpdateTimer = new QTimer(this);
    _streamUpdateTimer->setInterval(50);
    _streamUpdateTimer->setSingleShot(true);
    connect(_streamUpdateTimer, &QTimer::timeout, this, [this]()
    {
        applyTranslationWithFixedScroll();
    });
}

ITranslateWidget::~ITranslateWidget()
{
    detachTrUnit();
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
    applyTranslationWithFixedScroll();
}

void ITranslateWidget::detachTrUnit() const
{
    if (_trUnit)
    {
        _trUnit->detachDisplayWidget();
    }
}

void ITranslateWidget::abortTrUnit() const
{
    if (_trUnit)
    {
        _trUnit->abortTranslateRequest();
    }
}

void ITranslateWidget::setTrUnit(TranslateUnit* inTrUnit)
{
    _trUnit = inTrUnit;
}

void ITranslateWidget::applyTranslationWithFixedScroll()
{
    const int prevVerticalScrollVal   = getVerticalScrollBar()->value();
    const int prevHorizontalScrollVal = getHorizontalScrollBar()->value();
    const int prevTextCursorPos       = getTextCursor().position();

    applyTranslation();

    // 스트리밍 간 커서 현재 위치 유지
    QTextCursor textCursor = getTextCursor();
    textCursor.setPosition(prevTextCursorPos);
    setTextCursor(textCursor);

    // 스크롤바 현재 위치 유지.
    // 커서보다 나중에 적용해야, 커서 위치가 아닌 곳으로 스크롤할 수 있습니다.
    getVerticalScrollBar()->setValue(prevVerticalScrollVal);
    getHorizontalScrollBar()->setValue(prevHorizontalScrollVal);
}
