// Copyright (c) 2026 Kim Yubin. All rights reserved.
#ifndef SOLTRANSLATOR_HISTORYCACHEDATA_H
#define SOLTRANSLATOR_HISTORYCACHEDATA_H

#include <SolTypes.h>

#include <QtTypes>
#include <qnamespace.h>
#include <QString>

enum class TextStyle;

class TrHistoryCacheData
{
public:
    explicit TrHistoryCacheData();

    // order by db table and sql select 
    // create_history_data.sql /  select_translation_timeline.sql
    explicit TrHistoryCacheData(const qint64 inDbId
                            , const QString& inEngine
                            , const QString& inSourceLang
                            , const QString& inTargetLang
                            , const QString& inSourceText
                            , const QString& inTargetText
                            , TextStyle inTextStyle
                            , const QString& inTimeStamp
                            , Qt::CheckState inCheckState = Qt::Unchecked);


    qint64 getDbId() const;
    TextStyle getTextStyle() const;
    Qt::CheckState getCheckState() const;
    void setCheckState(const Qt::CheckState inState);

    QString getEngine() const;
    QString getSourceLang() const;
    QString getTargetLang() const;
    QString getSourceText() const;
    QString getTargetText() const;

    QString getTimeStamp() const;

private:
    qint64 _dbId;
    QString _engine;
    QString _sourceLang;
    QString _targetLang;
    QString _sourceText;
    QString _targetText;
    TextStyle _textStyle;
    QString _timeStamp;

    Qt::CheckState _bCheckState;
};

#endif //SOLTRANSLATOR_HISTORYCACHEDATA_H
