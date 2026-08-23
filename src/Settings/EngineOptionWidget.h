// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef ENGINEOPTIONWIDGET_H
#define ENGINEOPTIONWIDGET_H

#include "IOptionWidget.h"
#include "Types/SolExpected.hpp"
#include "Types/SolTypes.h"

class QTabWidget;

namespace Sol
{
class OptionGroupBox;
class OptionKey;
class ITranslateEngine;
struct OptionSpec;
class SettingCard;
class EngineId;
class SolTranslatorCore;

class EngineOptionWidget : public IOptionWidget
{
    Q_OBJECT

public:
    explicit EngineOptionWidget(QWidget* inParent = nullptr);
    ~EngineOptionWidget() override;

protected:
    void addEngineSettings(const ITranslateEngine* inEngine);

private:
    void showErrorMessage(const Error& inError);

    /**
     * Get the stored engine option value
     * and create the setOption functor.
     *
     * @tparam T load/store type
     * @return stored value, setOption functor.
     */
    template <typename T>
    Expected<std::tuple<T, Callback<void(const T&)>>> makeSetOption(const EngineId& inEngineId
                                                                  , const OptionKey& inKey);

    void stringSaverCard(OptionGroupBox* inOptGroup
                       , const EngineId& inEngineId
                       , const OptionSpec& inOptSpec);

    void doubleSpinCard(OptionGroupBox* inOptGroup
                      , const EngineId& inEngineId
                      , const OptionSpec& inOptSpec);

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


} // namespace Sol

#endif //ENGINEOPTIONWIDGET_H
