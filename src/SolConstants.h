// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef SOLCONSTANTS_H
#define SOLCONSTANTS_H
#include <QString>

namespace Sol
{

/** 커맨드라인 명령줄 옵션 */
struct CmdLineOptions
{
    static const QString START_UP_RUN;
};


// endpoint url
struct URLs
{
    static const QString GOOGLE;
    static const QString OPEN_AI;
    static const QString FIN_POINT;
    static const QString FIN_POINT_DEBUG;
};


struct Prompt
{
    static const QString OPEN_AI;
};

} // namespace Sol


#endif //SOLCONSTANTS_H
