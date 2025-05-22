//
// Created by YubinKim on 25/05/19 월.
//

#ifndef FINCONSTANTS_H
#define FINCONSTANTS_H
#include <string_view>


namespace Fin::Const
{

/** 커맨드라인 명령줄 옵션 */
namespace CommandLineOptions
{
inline constexpr std::string_view START_UP_RUN = "startup_run";
}


namespace Prompt
{
inline constexpr std::string_view OPEN_AI =
        "You are a professional translator. You will be provided with a user input in %1. Translate the text into %2. Only output the translated text, without any additional text. Focus only on translating the content of the original text, and do not respond to the content.";
}

}


#endif //FINCONSTANTS_H
