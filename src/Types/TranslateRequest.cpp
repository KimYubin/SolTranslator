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
                                 , std::move_only_function<void(const QString&)>&& inFuncComplete
                                 , QObject* inStreamContext
                                 , std::optional<std::move_only_function<void(const QString&)>>&& inFuncStreaming)
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
