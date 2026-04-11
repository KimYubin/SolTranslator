// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_HISTORYCACHEDATA_H
#define SOLTRANSLATOR_HISTORYCACHEDATA_H

#include "SolTypes.h"

#include <QDateTime>
#include <QString>
#include <Qt>
#include <QtTypes>

class HistoryCacheData
{
public:
    explicit HistoryCacheData();

    // create_history_data.sql /  select_translation_timeline.sql
    explicit HistoryCacheData(const qint64 inDbId
                            , const QString& inEngine
                            , const QString& inSourceLang
                            , const QString& inTargetLang
                            , const QString& inSourceText
                            , const QString& inTargetText
                            , const qint64 inTimelineId
                            , const qint64 inTimeStamp
                            , const TextStyle inTextStyle
                            , const Qt::CheckState inCheckState = Qt::Unchecked);


    qint64 getDbId() const;
    QString getEngine() const;
    QString getSourceLang() const;
    QString getTargetLang() const;
    QString getSourceText() const;
    QString getTargetText() const;
    QString getText(const TextCategory inTextCategory) const;
    /** simplified Source text for preview. */
    QString getSourceSimplifiedText() const;
    /** 미리보기용 간략화된 번역문을 반환합니다. */
    QString getTargetSimplifiedText() const;

    qint64 getTimelineId() const;
    QDateTime getTimeStamp() const;
    QString getTimeStampString() const;

    TextStyle getTextStyle() const;
    Qt::CheckState getCheckState() const;
    void setCheckState(const Qt::CheckState inState);

private:
    qint64 _dbId;
    QString _engine;
    QString _sourceLang;
    QString _targetLang;
    QString _sourceText;
    QString _targetText;

    qint64 _timelineId;
    QDateTime _timeStamp;

    TextStyle _textStyle;
    Qt::CheckState _bCheckState;
};

#endif //SOLTRANSLATOR_HISTORYCACHEDATA_H
