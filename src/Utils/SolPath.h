// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_SOLPATHS_H
#define SOLTRANSLATOR_SOLPATHS_H

class QString;

namespace Sol
{
enum class SolFile;

class SolPath
{
public:

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

} // namespace Sol

#endif //SOLTRANSLATOR_SOLPATHS_H