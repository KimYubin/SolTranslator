// SPDX-FileCopyrightText: Copyright (C) 2026 Kim Yubin. All rights reserved.

#ifndef SOLTRANSLATOR_TR_H
#define SOLTRANSLATOR_TR_H

class QString;
enum class Tr;

namespace Sol
{
/**
 * 
 */
QString i18n(const Tr inTr);
} // namespace Sol

enum class Tr
{
    Translation_Engine
  , Save
  , Translation_Engine_Settings
  , Ai_Options
  , OpenAi_Temperature
  , Default_Value_Hint
  , General
  , Appearance_Behavior
  , Run_On_Start
  , Run_On_Start_Desc
  , Remember_Geometry
  , Remember_Geometry_Desc
  , Apply
  , Apply_Theme
  , Apply_Theme_Desc
  , Popup_Translation
  , Popup_Target_Language_Desc
  , Popup_Temp_Window
  , Popup_Temp_Window_Desc
  , Source_Target_Toggle
  , Delete_Translation
  , Language_Search
  , Sol
  , Always_On_Top_Off
  , Always_On_Top_On
  , Temp_Window_Mode
  , Normal_Window_Mode
  , Minimize
  , Restore_Previous_Size
  , Maximize
  , Close
  , Register_Failed
  , Unregister_Failed
  , Settings
  , Sol_Translator
  , Text
  , Preparing
  , Document
  , History
  , Select_Translation_Engine
  , First_To_Tray_Noti
  , First_To_Tray_Message
  , Confirm_Quit
  , Quit
  , Cancel
  , Tray_Menu_Minimize
  , Tray_Menu_Restore
  , Tray_Menu_Settings
  , Tray_Menu_Quit
  , Copy_Translation
  , Copy_Complete_Noti
  , Source_Text_Editor
  , Translation_Result
  , Source_Language
  , Target_Language
  , Swap_Language
  , Swap_Language_Desc
  , Re_Translate
  , Translating

  , Size
};

#endif //SOLTRANSLATOR_TR_H
