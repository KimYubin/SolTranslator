// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "TranslateRequest.h"

#include "Widgets/ITranslateWidget.h"


TranslateRequest::TranslateRequest(ITranslateWidget* inTrDisplayWidget
                                 , const bool inIsIgnoreCache
                                 , const EngineId& inEngineId
                                 , const QString& inSourceText
                                 , const TextStyle inTextFormat
                                 , const LangType inSourceLang
                                 , const LangType inTargetLang
                                 , QObject* inCompleteContext
                                 , ReceiveMoveFunc&& inFuncComplete
                                 , QObject* inStreamContext
                                 , std::optional<ReceiveMoveFunc>&& inFuncStreaming)
    : trDisplayWidget(inTrDisplayWidget)
    , isIgnoreCache(inIsIgnoreCache)
    , engineId(inEngineId)
    , sourceText(inSourceText)
    , textFormat(inTextFormat)
    , sourceLang(inSourceLang)
    , targetLang(inTargetLang)
    , completeContext(inCompleteContext)
    , callbackTranslateComplete(std::move(inFuncComplete))
    , streamContext(inStreamContext)
    , callbackTranslateStreaming(std::move(inFuncStreaming))
{}
