// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "AbstractManager.h"

class EngineId;
class QSettings;
enum class Action;
enum class ScreenPopupPolicy;
enum class LangType;

#if defined(solConfig)
#undef solConfig
#endif
#define solConfig (*(SolTranslatorCore::instance()->manager<ConfigManager>()))

/**
 * The ConfigManager class stores and manages configuration/settings.
 */
class ConfigManager: public AbstractManager
{
    Q_OBJECT

public:
    explicit ConfigManager(SolTranslatorCore* parent);

    void setCurrentEngineId(const EngineId& inEngineId);
    EngineId currentEngineId() const;

    void setApiKey(const EngineId& inEngineId, const QString& inAPIKey);
    QString apiKey(const EngineId& inEngineId) const;

    // AI setup
    void setAiModel(const EngineId& inEngineId, const QString& inModelName);
    QString AiModel(const EngineId& inEngineId) const;

    void setAi_Temperature(const EngineId& inEngineId, const double inTemperature);
    double Ai_Temperature(const EngineId& inEngineId) const;

    void setStartRun(const bool inStartRun);
    bool startRun() const;

    /** 팝업 번역 도착 언어 */
    void setPopupTargetLang(const LangType inLangType);
    LangType popupTargetLang() const;

    /** TextEdit 번역 출발 언어 */
    void setTextSrcLang(const LangType inLangType);
    LangType textSrcLang() const;

    /** TextEdit 번역 도착 언어 */
    void setTextTargetLang(const LangType inLangType);
    LangType textTargetLang() const;

    void setPopupGeometry(const QRect& inGeo);
    QRect popupGeometry() const;
    void setPopupScreenPolicy(const ScreenPopupPolicy& inPolicy);
    ScreenPopupPolicy popupScreenPolicy() const;

    /** 창의 위치와 크기를 기억 유무를 저장합니다. */
    void setIsRememberWindowGeometry(const bool inIsRememberWindowGeometry);
    bool isRememberWindowGeometry() const;

    /** 창의 위치와 크기를 저장합니다. */
    void saveWidgetGeometry(const QWidget* inWidget);
    bool restoreWidgetGeometry(QWidget* inWidget) const;

    /**
     * 팝업 번역창의 임시창 유무.
     * 임시창이면 포커스 이탈시, 창이 자동으로 닫힙니다.
     * 기본값은 false입니다.
     */
    void setIsPopupTrWindowTemp(const bool inTemp);
    bool isPopupTrWindowTemp() const;

    /** 메인 창을 처음 닫은 후 호출합니다.*/
    void setFirstCloseToTray();

    /** 메인 창을 처음 닫았나요? 그렇다면, 앱이 트레이로 숨겨졌음을 안내해야 합니다. */
    bool isFirstCloseToTray() const;

    void setHistoryTimeFormat(const QString& inFormat);
    QString historyTimeFormat() const;

    // ~======================
    // shortcut
    void setShortCut(const Action inShortCut, const QKeySequence& inKeySequence);
    QKeySequence shortcut(const Action inShortCut) const;

private:
    void setSaveGeometry(const QString& inKey, const QByteArray& inGeoData);
    std::tuple<bool, QByteArray> saveGeometry(const QString& inKey) const;

private:
    QSettings* _settings;
};

#endif //CONFIGMANAGER_H
