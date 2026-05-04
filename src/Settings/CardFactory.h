// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_CARDFACTORY_H
#define SOLTRANSLATOR_CARDFACTORY_H

#include "Types/OptionData.h"

#include <QObject>

#include <expected>

class SettingCard;

class CardFactory : public QObject
{
    Q_OBJECT

public:
    static SettingCard* createBaseCard(QWidget* inContent
                                     , QWidget* inParent
                                     , const QString& inHeader
                                     , const std::optional<QString>& inDescription);

    static std::expected<SettingCard*, QString> createDoubleSpin(QWidget* inParent
                                                               , const OptionData& inOptData
                                                               , const double inCurrentVal);
};


#endif //SOLTRANSLATOR_CARDFACTORY_H
