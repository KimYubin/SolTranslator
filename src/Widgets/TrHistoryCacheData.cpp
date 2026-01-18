// Copyright (c) 2026 Kim Yubin. All rights reserved.
#include "TrHistoryCacheData.h"


TrHistoryCacheData::TrHistoryCacheData()
    : _dbId(0)
    , _textStyle(TextStyle::PlainText)
    , _bCheckState(Qt::Unchecked) {}

TrHistoryCacheData::TrHistoryCacheData(const qint64 inDbId
                                 , const QString& inEngine
                                 , const QString& inSourceLang
                                 , const QString& inTargetLang
                                 , const QString& inSourceText
                                 , const QString& inTargetText
                                 , TextStyle inTextStyle
                                 , const QString& inTimeStamp
                                 , Qt::CheckState inCheckState)
    : _dbId(inDbId)
    , _engine(inEngine)
    , _sourceLang(inSourceLang)
    , _targetLang(inTargetLang)
    , _sourceText(inSourceText)
    , _targetText(inTargetText)
    , _textStyle(inTextStyle)
    , _timeStamp(inTimeStamp)
    , _bCheckState(inCheckState)
{}

qint64 TrHistoryCacheData::getDbId() const
{
    return _dbId; 
}
TextStyle TrHistoryCacheData::getTextStyle() const
{
    return _textStyle; 
}
Qt::CheckState TrHistoryCacheData::getCheckState() const
{
    return _bCheckState; 
}

void TrHistoryCacheData::setCheckState(const Qt::CheckState inState) 
{
    _bCheckState = inState;    
}

QString TrHistoryCacheData::getEngine() const
{
    return _engine; 
}
QString TrHistoryCacheData::getSourceLang() const
{
    return _sourceLang; 
}
QString TrHistoryCacheData::getTargetLang() const
{
    return _targetLang; 
}
QString TrHistoryCacheData::getSourceText() const
{
    return _sourceText; 
}
QString TrHistoryCacheData::getTargetText() const
{
    return _targetText; 
}
QString TrHistoryCacheData::getTimeStamp() const
{
    return _timeStamp; 
}