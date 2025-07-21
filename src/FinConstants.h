// SPDX-FileCopyrightText: Copyright (C) 2025 Kim Yubin. All rights reserved.

#ifndef FINCONSTANTS_H
#define FINCONSTANTS_H
#include <QString>

namespace Fin
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

}


#endif //FINCONSTANTS_H
