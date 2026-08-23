// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATORCORE_H
#define SOLTRANSLATORCORE_H

#include <QObject>
#include <QPointer>

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#if defined(solCore)
#undef solCore
#endif
#define solCore (SolTranslatorCore::instance())

namespace Sol
{
class AbstractManager;
class SolMainWidget;

/**
 * SolTranslator의 Non-UI 관련 기능과 mainWidget을 관리하는 최상위 객체입니다.
 * 객체는 전역에서 유일하게 존재해야 합니다.
 */
class SolTranslatorCore : public QObject
{
    Q_OBJECT

    static SolTranslatorCore* _self;

public:
    explicit SolTranslatorCore(QObject* inParent = nullptr);
    ~SolTranslatorCore() override;

    static SolTranslatorCore* instance() noexcept { return _self; }

public:
    template <std::derived_from<AbstractManager> T, typename... Args>
    T* emplaceManager(Args&&... args)
    {
        Q_ASSERT_X(_managers.contains(std::type_index(typeid(T))) == false, "manager", "Manager already registered");

        auto emplaceRes = _managers.emplace(std::type_index(typeid(T)), std::make_unique<T>(std::forward<Args>(args)...));

        return static_cast<T*>(emplaceRes.first->second.get());
    }

    template <std::derived_from<AbstractManager> T>
    void registerManager(std::unique_ptr<T>&& inManager)
    {
        Q_ASSERT_X(_managers.contains(std::type_index(typeid(T))) == false, "manager", "Manager already registered");

        _managers[std::type_index(typeid(T))] = std::move(inManager);
    }

    template <std::derived_from<AbstractManager> T>
    T* manager()
    {
        const auto it = _managers.find(std::type_index(typeid(T)));

        Q_ASSERT_X(it != _managers.end(), "manager", "Access an unregistered manager.");

        return static_cast<T*>(it->second.get());
    }

    void postInitialize();

    void raiseMainWidget();

private:
signals:
    void postInitialized();

private:
    std::unordered_map<std::type_index, std::unique_ptr<AbstractManager>> _managers;

    QPointer<SolMainWidget> _solMainWidget;
};
} // namespace Sol

#endif //SOLTRANSLATORCORE_H
