// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ENGINEOPTIONWIDGET_H
#define ENGINEOPTIONWIDGET_H

#include "IOptionWidget.h"
#include "Types/SolExpected.hpp"

class OptionGroupBox;
class OptionKey;
class QTabWidget;
class ITranslateEngine;
struct OptionData;
class SettingCard;
class EngineId;
class SolTranslatorCore;

template <typename T>
using MoveFunc = std::move_only_function<T>;

class EngineOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit EngineOptionWidget(QWidget* parent = nullptr);
    ~EngineOptionWidget() override;

protected:
    void addEngineSettings(const ITranslateEngine* inEngine);

private:
    void showErrorMessage(const Error& inError);

    /**
     * Get the stored engine attribute value
     * and create the setAttribute functor.
     *
     * @tparam T load/store type
     * @return stored value, setAttribute functor.
     */
    template <typename T>
    Expected<std::tuple<T, MoveFunc<void(const T&)>>> makeSetAttribute(const EngineId& inEngineId
                                                                     , const OptionKey& inKey);

    void stringSaverCard(OptionGroupBox* inOptGroup
                       , const EngineId& inEngineId
                       , const OptionData& inOptData);

    void doubleSpinCard(OptionGroupBox* inOptGroup
                      , const EngineId& inEngineId
                      , const OptionData& inOptData);

private:
    friend class EngineOptionPage;

    QTabWidget* _tabWidget;
};


class EngineOptionPage : public IOptionPage
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(EngineOptionPage)

public:
    EngineOptionPage();
    ~EngineOptionPage() override;
};


#endif //ENGINEOPTIONWIDGET_H
