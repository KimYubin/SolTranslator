// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef TRANSLATEMANAGER_H
#define TRANSLATEMANAGER_H


#include "AbstractManager.h"
#include "SolTypes.h"


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

    TranslateUnit* executeNewTranslateUnit(TranslateRequestInfo&& inTranslateRequestInfo);

    QPointer<TranslateUnit> translateText(TranslateRequestInfo&& inTranslateRequestInfo);

    void translateAtPopup(const QMimeData* inMimeData
                        , const LangType inSourceLang
                        , const LangType inTargetLang);

public:
    QNetworkAccessManager* getNetworkAccessManager() const { return _networkAccessManager; };

private:
    QNetworkAccessManager* _networkAccessManager;

};



#endif //TRANSLATEMANAGER_H

