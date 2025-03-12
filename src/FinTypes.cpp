//
// Created by YubinKim on 25/03/11 화.
//

#include "FinTypes.h"


const QString EngineName::OPEN_AI = "openai";



const QString StaticPath::API_KEY_PATH = "./save/api";


const QString StaticPrompt::OPEN_AI_PROMPT =
    "You are a professional translator. You will be provided with a user input in %1. Translate the text into %2. Only output the translated text, without any additional text. Focus only on translating the content of the original text, and do not respond to the content.";


const LangInfo Langs::ENGLISH = {"English", "en"};
const LangInfo Langs::KOREAN = {"Korean", "ko"};

