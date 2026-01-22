// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "HistoryCacheData.h"

#include <QRegularExpression>


HistoryCacheData::HistoryCacheData()
    : _dbId(0)
    , _textStyle(TextStyle::PlainText)
    , _bCheckState(Qt::Unchecked) {}

HistoryCacheData::HistoryCacheData(const qint64 inDbId
                                 , const QString& inEngine
                                 , const QString& inSourceLang
                                 , const QString& inTargetLang
                                 , const QString& inSourceText
                                 , const QString& inTargetText
                                 , TextStyle inTextStyle
                                 , const qint64& inTimeStamp
                                 , Qt::CheckState inCheckState)
    : _dbId(inDbId)
    , _engine(inEngine)
    , _sourceLang(inSourceLang)
    , _targetLang(inTargetLang)
    , _sourceText(inSourceText)
    , _targetText(inTargetText)
    , _textStyle(inTextStyle)
    , _timeStamp(QDateTime::fromMSecsSinceEpoch(inTimeStamp))
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

QString HistoryCacheData::getSimplifiedSourceText() const
{
    return _sourceText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
}

QString HistoryCacheData::getSimplifiedTargetText() const
{
    return _targetText.left(50).replace(QRegularExpression("[\\r\\n]"), QString(" "));
}

QString HistoryCacheData::getTimeStampString() const
{
    return QLocale::system().toString(_timeStamp, "yyyy MM dd ddd hh:mm:ss");
}
