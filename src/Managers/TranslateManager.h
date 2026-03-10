// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H


#include "AbstractManager.h"


enum class TextStyle;
struct TranslateRequestInfo;
class QNetworkAccessManager;
class QTextEdit;
class TranslateUnit;
class SolTranslatorCore;
class QMimeData;


class TranslateManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit TranslateManager(SolTranslatorCore* parent);
    virtual void postInitialize() override;

    TranslateUnit* executeNewTranslateUnit(TranslateRequestInfo&& inTranslateRequestInfo);

    QPointer<TranslateUnit> translateText(TranslateRequestInfo&& inTranslateRequestInfo);

    void translateAtPopup(const QString& inOriginText
                        , const TextStyle inTextStyle);

public:
    QNetworkAccessManager* getNetworkAccessManager() const { return _networkAccessManager; };

private:
    void processPopupTranslate();

    QNetworkAccessManager* _networkAccessManager;

};



#endif //TRANSLATEMANAGER_H

