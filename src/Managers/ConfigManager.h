// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "AbstractManager.h"
#include "SecretStore.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"

#if defined(solConfig)
#undef solConfig
#endif
#define solConfig (*(SolTranslatorCore::instance()->manager<ConfigManager>()))

class QSettings;

namespace Sol
{
class SecretStore;
struct OptionSpec;
class OptionKey;
class EngineId;
enum class Action;
enum class ScreenPopupPolicy;
enum class LangType;

/**
 * The ConfigManager class stores and manages configuration/settings.
 */
class ConfigManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit ConfigManager(SolTranslatorCore* inParent);

    /**
     * Load The secret key, asynchronously.
     * The secretKey() should be called after the callback.
     *
     * @param inKey The key of the secret key.
     * @param inCallback Callback to be called after the load is complete.
     */
    void loadSecretKey(const QString& inKey
                     , LoadCallback&& inCallback);

    /**
     * Load The secret key of the engine, asynchronously.
     * The secretKey() should be called after the callback.
     *
     * @param inEngineId Target engine.
     * @param inOptionKey The key of the secret key.
     * @param inCallback Callback to be called after the load is complete.
     */
    void loadSecretKey(const EngineId& inEngineId
                     , const OptionKey& inOptionKey
                     , LoadCallback&& inCallback);

    void loadSecretKeys(const std::vector<QString>& inKeyList
                      , Callback<void()>&& inCallback);

    void setSecretKey(const QString& inKey
                    , const QVariant& inValue
                    , Callback<void()>&& inCallback = []() {});

    /**
     * Get the cached secret key.
     * It must be called after loadSecretKey() has been invoked;
     * otherwise, it will return the default or an empty value.
     *
     * @param inKey 
     * @param inDefault 
     * @return Cached secret key. If it is not cached, the default or an empty value.
     * @see loadSecretKey()
     */
    QVariant secretKey(const QString& inKey, const QVariant& inDefault) const;


    Expected<void> setEngineOption(const EngineId& inEngineId, const OptionKey& inKey, const QVariant& inValue);
    Expected<QVariant> engineOption(const EngineId& inEngineId, const OptionKey& inKey) const;

    void setCurrentEngineId(const EngineId& inEngineId);
    EngineId currentEngineId() const;


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

    void setIsShowToolTipShortcut(const bool inIsShow);
    bool isShowToolTipShortcut() const;

    // ~======================
    // shortcut
    void setShortCut(const Action inShortCut, const QKeySequence& inKeySequence);
    QKeySequence shortcut(const Action inShortCut) const;

private:
    void setSaveGeometry(const QString& inKey, const QByteArray& inGeoData);
    std::tuple<bool, QByteArray> saveGeometry(const QString& inKey) const;

private:
    QSettings* _settings;
    SecretStore* _secretStore;
};

} // namespace Sol

#endif //CONFIGMANAGER_H
