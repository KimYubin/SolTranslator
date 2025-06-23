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

namespace URLs
{
inline constexpr std::string_view GOOGLE    = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=%1&tl=%2&dt=t&q=%3";
inline constexpr std::string_view OPEN_AI   = "https://api.openai.com/v1/chat/completions";
inline constexpr std::string_view FIN_POINT
                // = "https://us-central1-fintrans-33fftt.cloudfunctions.net/helloWorld/v1/text";
                = "https://asia-northeast3-fintrans-33fftt.cloudfunctions.net/helloWorld/v1/text";
                // = "http://localhost:5001/fintrans-33fftt/us-central1/helloWorld/v1/text";
}

namespace Prompt
{
inline constexpr std::string_view OPEN_AI =
        "You are a professional translator. You will be provided with a user input in %1. Translate the text into %2. Only output the translated text, without any additional text. Focus only on translating the content of the original text, and do not respond to the content.   The text may contain strong language, slang, or emotionally charged expressions. Do not censor, soften, or omit any part of the text. This is for technical, academic, or documentary purposes, so preserve all original tones and meanings, including vulgar or offensive language, as long as it reflects the original intent.";

}

}


#endif //FINCONSTANTS_H
