// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_TranslateRequest_H
#define SOLTRANSLATOR_TranslateRequest_H

#include "EngineId.h"
#include "SolTypes.h"

#include <QPointer>
#include <QString>

class ITranslateWidget;

using ReceiveMoveFunction = std::move_only_function<void(const QString&)>;

struct TranslateRequest
{
    TranslateRequest() = default;

    /**
     * 번역 요청에 필요한 정보를 모아놓은 구조체입니다.
     * 
     * @param inTrDisplayWidget trUnit의 번역값을 표기하는 ITranslateWidget입니다.
     * @param inIsIgnoreCache true면 캐시에서 찾지 않고 번역을 요청합니다. 재번역시 사용됩니다.
     * @param inEngineId 번역엔진 종류. 엔진 종류가 다르면, 기록에서도 다른 번역으로 취급됩니다.
     * @param inSourceText 번역 원문
     * @param inTextFormat 원문 텍스트의 종류.
     * @param inSourceLang 출발 언어
     * @param inTargetLang 도착 언어
     * @param inCompleteContext callbackTranslateComplete 수명을 관리하는 객체입니다.
     * @param inFuncComplete 번역이 완료되면 호출되는 콜백입니다.
     * @param inStreamContext callbackTranslateStreaming 수명을 관리하는 객체입니다.
     * @param inFuncStreaming 번역 스트리밍 중간 값들을 받는 콜백입니다. 여러번 호출됩니다.
     */
    TranslateRequest(ITranslateWidget* inTrDisplayWidget
                   , const bool inIsIgnoreCache
                   , const EngineId& inEngineId
                   , const QString& inSourceText
                   , const TextStyle inTextFormat
                   , const LangType inSourceLang
                   , const LangType inTargetLang
                   , QObject* inCompleteContext
                   , ReceiveMoveFunction&& inFuncComplete
                   , QObject* inStreamContext = nullptr
                   , std::optional<ReceiveMoveFunction>&& inFuncStreaming = std::nullopt);

    QPointer<ITranslateWidget> trDisplayWidget;
    bool isIgnoreCache;
    EngineId engineId;
    QString sourceText;
    TextStyle textFormat;
    LangType sourceLang;
    LangType targetLang;
    QPointer<QObject> completeContext;
    ReceiveMoveFunction callbackTranslateComplete;
    QPointer<QObject> streamContext;
    std::optional<ReceiveMoveFunction> callbackTranslateStreaming;
};


#endif //SOLTRANSLATOR_TranslateRequest_H
