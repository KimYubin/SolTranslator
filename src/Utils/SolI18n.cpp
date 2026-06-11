// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#include "SolI18n.h"

#include <QCoreApplication>
#include <QString>

namespace
{
class TrImpl
{
    Q_DECLARE_TR_FUNCTIONS(Tr)
};

} // anonymous namespace


QString Sol::i18n(const Tr inTr)
{
    using enum Tr;

    switch (inTr)
    {
    case Translation_Engine:             return TrImpl::tr("번역 엔진");
    case Save:                           return TrImpl::tr("저장");
    case Translation_Engine_Settings:    return TrImpl::tr("번역 엔진 설정");
    case Options:                        return TrImpl::tr("옵션");
    case Temperature_Option:             return TrImpl::tr("온도 설정", "AI 온도 설정.");
    case Default_Value_Hint:             return TrImpl::tr("(기본값: %1)");
    case Api_Key:                        return TrImpl::tr("API 키");
    case General:                        return TrImpl::tr("일반");
    case Appearance_Behavior:            return TrImpl::tr("모양 및 동작");
    case Run_On_Start:                   return TrImpl::tr("시작 시 실행");
    case Run_On_Start_Desc:              return TrImpl::tr("시스템 시작 시 Sol번역기가 자동으로 실행됩니다.");
    case Remember_Geometry:              return TrImpl::tr("창 위치, 크기 기억");
    case Remember_Geometry_Desc:         return TrImpl::tr("다시 시작할 때, 이전 창의 위치와 크기로 복원합니다.");
    case Show_Shortcut_ToolTip:          return TrImpl::tr("툴팁에 단축키 표시");
    case Show_Shortcut_ToolTip_Desc:     return TrImpl::tr("툴팁에 단축키가 함께 표시됩니다.");
    case Apply:                          return TrImpl::tr("적용");
    case Apply_Theme:                    return TrImpl::tr("테마 적용");
    case Apply_Theme_Desc:               return TrImpl::tr("테마를 적용합니다.");
    case Popup_Translation:              return TrImpl::tr("팝업 번역");
    case Popup_Target_Language_Desc:     return TrImpl::tr("팝업 번역의 목표가 되는 언어를 선택합니다.");
    case Popup_Temp_Window:              return TrImpl::tr("팝업 번역창을 임시창으로 열기");
    case Popup_Temp_Window_Desc:         return TrImpl::tr("팝업 번역창이 임시창으로 생성됩니다. 번역 중 다른 곳을 클릭하면 번역창이 닫힙니다.");
    case Source_Target_Toggle:           return TrImpl::tr("원문/번역 토글");
    case Delete_Translation:             return TrImpl::tr("번역 삭제", "번역 기록을 삭제합니다.");
    case View_In_Popup:                  return TrImpl::tr("팝업창에서 보기");
    case Language_Search:                return TrImpl::tr("언어 검색", "번역 출발, 도착 언어를 찾는 검색창");
    case Sol:                            return TrImpl::tr("Sol");
    case Always_On_Top_Off:              return TrImpl::tr("항상 위 끄기");
    case Always_On_Top_On:               return TrImpl::tr("항상 위 켜기");
    case Temp_Window_Mode:               return TrImpl::tr("임시창 모드");
    case Normal_Window_Mode:             return TrImpl::tr("일반창 모드");
    case Minimize:                       return TrImpl::tr("최소화");
    case Restore_Previous_Size:          return TrImpl::tr("이전 크기로 복원");
    case Maximize:                       return TrImpl::tr("최대화");
    case Close:                          return TrImpl::tr("닫기");
    case Register_Failed:                return TrImpl::tr("Failed to register %1. Error: %2");
    case Unregister_Failed:              return TrImpl::tr("Failed to unregister %1. Error: %2");
    case Settings:                       return TrImpl::tr("설정", "Settings나 Options");
    case Sol_Translator:                 return TrImpl::tr("SolTranslator");
    case Text:                           return TrImpl::tr("텍스트", "텍스트 번역 탭의 이름");
    case Preparing:                      return TrImpl::tr("준비 중", "미완성된 기능에 대한 안내입니다.");
    case Document:                       return TrImpl::tr("문서", "문서 번역 탭의 이름");
    case History:                        return TrImpl::tr("기록", "이전 번역 기록");
    case Select_Translation_Engine:      return TrImpl::tr("번역 엔진 선택");
    case First_To_Tray_Noti:             return TrImpl::tr("트레이로 최소화되었습니다.");
    case First_To_Tray_Message:          return TrImpl::tr("Sol 번역기가 아직 실행 중입니다.\n""아이콘을 클릭하여 다시 실행하거나, 종료할 수 있습니다.");
    case Confirm_Quit:                   return TrImpl::tr("정말 종료할까요?");
    case Quit:                           return TrImpl::tr("종료");
    case Cancel:                         return TrImpl::tr("취소");
    case Tray_Menu_Minimize:             return TrImpl::tr("트레이로 최소화(&M)");
    case Tray_Menu_Restore:              return TrImpl::tr("창 복원(&R)");
    case Tray_Menu_Settings:             return TrImpl::tr("설정(&S)");
    case Tray_Menu_Quit:                 return TrImpl::tr("종료(&Q)");
    case Copy_Translation:               return TrImpl::tr("번역 복사");
    case Copy_Complete_Noti:             return TrImpl::tr("복사 완료!");
    case Source_Text_Editor:             return TrImpl::tr("번역 원문 입력 편집기");
    case Translation_Result:             return TrImpl::tr("번역 결과");
    case Source_Language:                return TrImpl::tr("출발 언어");
    case Target_Language:                return TrImpl::tr("도착 언어");
    case Swap_Language:                  return TrImpl::tr("언어 바꾸기");
    case Swap_Language_Desc:             return TrImpl::tr("출발 언어와 도착 언어를 서로 바꿉니다. 출발언어가 \'자동 감지\'라면 사용할 수 없습니다.");
    case Re_Translate:                   return TrImpl::tr("다시 번역");
    case Translating:                    return TrImpl::tr("번역 중...", "번역 결과를 받기 전에 대기 중임을 안내합니다.");
    case GoogleTranslate:                return TrImpl::tr("구글 번역");
    case MicrosoftTranslate:             return TrImpl::tr("마이크로소프트 번역");
    case DeepL:                          return TrImpl::tr("DeepL");
    case Papago:                         return TrImpl::tr("파파고");
    case OpenAI:                         return TrImpl::tr("OpenAI");
    case Gemini:                         return TrImpl::tr("Gemini");

    case Size: Q_UNREACHABLE();
    // default: Should not be used. There must be a 'case' for every enum class member.
    }
    Q_UNREACHABLE();
}


namespace
{
constexpr int TrKeyCheck = 69;
static_assert(static_cast<int>(Tr::Size) == TrKeyCheck, "Tr changed: update i18n()");
} // anonymous namespace
