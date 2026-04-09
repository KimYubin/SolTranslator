// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLPATHS_H
#define SOLTRANSLATOR_SOLPATHS_H
#include "AbstractManager.h"


enum class SolFile;

class SolPath : public AbstractManager
{
public:
    explicit SolPath(SolTranslatorCore* parent);

    static QString absolute(const SolFile inPath);
};


enum class SolFile
{
    Log
  , Config
  , TranslateHistory
  , HistoryDB
  , Size
};

#endif //SOLTRANSLATOR_SOLPATHS_H