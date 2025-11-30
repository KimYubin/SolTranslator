// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H


#include "AbstractManager.h"
#include "FinTypes.h"


class QNetworkAccessManager;
class QTextEdit;
class TranslateUnit;
class FinTranslatorCore;
class QMimeData;


class TranslateManager : public AbstractManager
{
    Q_OBJECT

public:
    explicit TranslateManager(FinTranslatorCore* parent);

    TranslateUnit* executeNewTranslateUnit(const TranslateRequestInfo& inTranslateRequestInfo);
    
    QPointer<TranslateUnit> translateText(const TranslateRequestInfo& inTranslateRequestInfo);

    void translateAtPopup(const QMimeData* inMimeData
                        , const LangType inSourceLang
                        , const LangType inTargetLang);

public:
    QNetworkAccessManager* getNetworkAccessManager() const { return _networkAccessManager; };

private:
    QNetworkAccessManager* _networkAccessManager;

};



#endif //TRANSLATEMANAGER_H

