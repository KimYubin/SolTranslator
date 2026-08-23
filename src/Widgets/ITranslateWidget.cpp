// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "ITranslateWidget.h"

#include "SolTranslatorCore.h"
#include "EngineUnits/TranslateUnit.h"
#include "Managers/TranslateManager.h"
#include "SubWidgets/SolBaseTextEdit.h"
#include "Types/SolExpected.hpp"
#include "Types/TranslateRequest.h"
#include "Utils/SolDebug.h"

#include <QScrollBar>
#include <QTimer>

namespace Sol
{
ITranslateWidget::ITranslateWidget(QWidget* inParent, const Qt::WindowFlags inFlags)
    : QWidget(inParent, inFlags)
    , _targetText("")
    , _textStyle(TextStyle::PlainText)
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

void ITranslateWidget::executeTranslateImpl(const EngineId& inEngineId
                                          , const QString& inSourceText
                                          , const TextStyle inTextStyle
                                          , const LangType inSourceLang
                                          , const LangType inTargetLang
                                          , const bool inIsIgnoreCache)
{
    setSourceAndStyle(inSourceText, inTextStyle);

    Expected<QPointer<TranslateUnit>> trResExp
    = solCore->manager<TranslateManager>()->translateText(TranslateRequest{
        this
      , inIsIgnoreCache
      , inEngineId
      , inSourceText
      , inTextStyle
      , inSourceLang
      , inTargetLang
      , this
      , [this](const QString& inStr) { completeTranslateText(inStr); }
      , this
      , [this](const QString& inStr) { streamTranslateText(inStr); }
    });

    if (!trResExp)
    {
        solDebug << "Translation attempt failed:" << trResExp.error();
        return;
    }

    setTrUnit(trResExp.value());
}

void ITranslateWidget::streamTranslateText(const QString& inTargetText)
{
    _targetText = inTargetText;

    _streamUpdateTimer->start();
}

void ITranslateWidget::completeTranslateText(const QString& inTargetText)
{
    _targetText = inTargetText;

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
        _trUnit->abortRequest();
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

void ITranslateWidget::setSourceAndStyle(const QString& inSourceText
                                       , const TextStyle inTextStyle)
{
    _sourceText = inSourceText;
    _textStyle  = inTextStyle;
}
} // namespace Sol
