// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "HistoryCacheData.h"

#include "SolTranslatorCore.h"
#include "Managers/ConfigManager.h"

#include <QRegularExpression>

HistoryCacheData::HistoryCacheData()
    : _dbId(0)
    , _timelineId(-1)
    , _textStyle(TextStyle::PlainText)
    , _bCheckState(Qt::Unchecked)
{}

HistoryCacheData::HistoryCacheData(const qint64 inDbId
                                 , const QString& inEngine
                                 , const QString& inSourceLang
                                 , const QString& inTargetLang
                                 , const QString& inSourceText
                                 , const QString& inTargetText
                                 , const qint64 inTimelineId
                                 , const qint64 inTimeStamp
                                 , const TextStyle inTextStyle
                                 , const Qt::CheckState inCheckState)
    : _dbId(inDbId)
    , _engine(inEngine)
    , _sourceLang(inSourceLang)
    , _targetLang(inTargetLang)
    , _sourceText(inSourceText)
    , _targetText(inTargetText)
    , _timelineId(inTimelineId)
    , _timeStamp(QDateTime::fromMSecsSinceEpoch(inTimeStamp))
    , _textStyle(inTextStyle)
    , _bCheckState(inCheckState)
{}

qint64 HistoryCacheData::getDbId() const
{
    return _dbId;
}

TextStyle HistoryCacheData::getTextStyle() const
{
    return _textStyle;
}

Qt::CheckState HistoryCacheData::getCheckState() const
{
    return _bCheckState;
}

void HistoryCacheData::setCheckState(const Qt::CheckState inState)
{
    _bCheckState = inState;
}

QString HistoryCacheData::getEngine() const
{
    return _engine;
}

QString HistoryCacheData::getSourceLang() const
{
    return _sourceLang;
}

QString HistoryCacheData::getTargetLang() const
{
    return _targetLang;
}

QString HistoryCacheData::getSourceText() const
{
    return _sourceText;
}

QString HistoryCacheData::getTargetText() const
{
    return _targetText;
}

QString HistoryCacheData::getText(const TextCategory inTextCategory) const
{
    return (inTextCategory == TextCategory::SourceText)
               ? getSourceText()
               : getTargetText();
}

QString HistoryCacheData::getSourceSimplifiedText() const
{
    return _sourceText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
}

QString HistoryCacheData::getTargetSimplifiedText() const
{
    return _targetText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
}

qint64 HistoryCacheData::getTimelineId() const
{
    return _timelineId;
}

QDateTime HistoryCacheData::getTimeStamp() const
{
    return _timeStamp;
}

QString HistoryCacheData::getTimeStampString() const
{
    return QLocale::system().toString(_timeStamp, solConfig.historyTimeFormat());
}
