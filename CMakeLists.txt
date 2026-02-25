cmake_minimum_required(VERSION 3.30.5)

project(SolTranslator VERSION 0.0.1 LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
        Qt6 REQUIRED COMPONENTS
        Widgets
        Svg
        SvgWidgets
        Network
        Concurrent
        Core
        Sql
        Gui
        LinguistTools
)

qt_add_executable(
        SolTranslator
        MANUAL_FINALIZATION

        src/main.cpp

        src/SolConstants.cpp
        src/SolConstants.h
        src/SolDatabase.cpp
        src/SolDatabase.h
        src/SolGuard.cpp
        src/SolGuard.h
        src/SolHashQueue.cpp
        src/SolHashQueue.h
        src/SolLog.cpp
        src/SolLog.h
        src/SolTranslatorCore.cpp
        src/SolTranslatorCore.h
        src/SolTypes.cpp
        src/SolTypes.h
        src/SolUtilibrary.cpp
        src/SolUtilibrary.h
        src/QuickTextEdit.qml

        resource/image/image.qrc
        resource/sql/sql.qrc
        resource/theme/theme.qrc
        external/magic_enum.hpp

        src/EngineUnits/FinPoint/FinPointTrUnit.cpp
        src/EngineUnits/FinPoint/FinPointTrUnit.h
        src/EngineUnits/GoogleEngine/GoogleTrUnit.cpp
        src/EngineUnits/GoogleEngine/GoogleTrUnit.h
        src/EngineUnits/OpenAI/OpenAiTrUnit.cpp
        src/EngineUnits/OpenAI/OpenAiTrUnit.h
        src/EngineUnits/TranslateUnit.cpp
        src/EngineUnits/TranslateUnit.h

        src/Managers/AbstractManager.cpp
        src/Managers/AbstractManager.h
        src/Managers/AsyncManager.cpp
        src/Managers/AsyncManager.h
        src/Managers/ConfigManager.cpp
        src/Managers/ConfigManager.h
        src/Managers/DataManager.cpp
        src/Managers/DataManager.h
        src/Managers/GlobalHotKeyManager.cpp
        src/Managers/GlobalHotKeyManager.h
        src/Managers/HistoryManager.cpp
        src/Managers/HistoryManager.h
        src/Managers/InputSimulator.cpp
        src/Managers/InputSimulator.h
        src/Managers/InputSimulator_Impl_Linux.cpp
        src/Managers/InputSimulator_Impl_Mac.cpp
        src/Managers/InputSimulator_Impl_Win.cpp
        src/Managers/StyleManger.cpp
        src/Managers/StyleManger.h
        src/Managers/TranslateManager.cpp
        src/Managers/TranslateManager.h

        src/Settings/EngineOptionWidget.cpp
        src/Settings/EngineOptionWidget.h
        src/Settings/EngineOptionWidget.ui
        src/Settings/GeneralOptionWidget.cpp
        src/Settings/GeneralOptionWidget.h
        src/Settings/IOptionWidget.cpp
        src/Settings/IOptionWidget.h
        src/Settings/SettingsWidget.cpp
        src/Settings/SettingsWidget.h
        src/Settings/SettingsWidget.ui

        src/SubWidgets/CustomMenuTextEdit.cpp
        src/SubWidgets/CustomMenuTextEdit.h
        src/SubWidgets/DropdownMenu.cpp
        src/SubWidgets/DropdownMenu.h
        src/SubWidgets/EnginSelector.cpp
        src/SubWidgets/EnginSelector.h
        src/SubWidgets/KeySelectionList.cpp
        src/SubWidgets/KeySelectionList.h
        src/SubWidgets/LanguageSelector.cpp
        src/SubWidgets/LanguageSelector.h
        src/SubWidgets/LayoutTextEdit.cpp
        src/SubWidgets/LayoutTextEdit.h
        src/SubWidgets/LoadingBar.cpp
        src/SubWidgets/LoadingBar.h
        src/SubWidgets/ResultTextEdit.cpp
        src/SubWidgets/ResultTextEdit.h
        src/SubWidgets/SettingCard.cpp
        src/SubWidgets/SettingCard.h
        src/SubWidgets/SolToast.cpp
        src/SubWidgets/SolToast.h
        src/SubWidgets/SolToolTip.cpp
        src/SubWidgets/SolToolTip.h
        src/SubWidgets/SwitchButton.cpp
        src/SubWidgets/SwitchButton.h

        src/Support/WidgetInspector.cpp
        src/Support/WidgetInspector.h

        src/Widgets/HistoryCacheData.cpp
        src/Widgets/HistoryCacheData.h
        src/Widgets/HistoryListDelegate.cpp
        src/Widgets/HistoryListDelegate.h
        src/Widgets/HistoryModel.cpp
        src/Widgets/HistoryModel.h
        src/Widgets/HistoryWidget.cpp
        src/Widgets/HistoryWidget.h
        src/Widgets/ISolWidget.cpp
        src/Widgets/ISolWidget.h
        src/Widgets/ITranslateWidget.cpp
        src/Widgets/ITranslateWidget.h
        src/Widgets/PopupTranslateWidget.cpp
        src/Widgets/PopupTranslateWidget.h
        src/Widgets/PopupTranslateWidget.ui
        src/Widgets/SolMainWidget.cpp
        src/Widgets/SolMainWidget.h
        src/Widgets/SolMainWidget.ui
        src/Widgets/TextEditTranslateWidget.cpp
        src/Widgets/TextEditTranslateWidget.h
        src/Widgets/TextEditTranslateWidget.ui
        src/SubWidgets/SolWidgetFactory.cpp
        src/SubWidgets/SolWidgetFactory.h

)

# ============================
# appicon windows
if (WIN32)
    set(app_icon_resource_windows "resource/image/icon/SolTranslator.rc")
    target_sources(SolTranslator PRIVATE ${app_icon_resource_windows})
endif ()

# Android 환경일 경우에만 패키지 소스 디렉토리 설정
# Define target properties for Android with Qt 6 as:
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
if (ANDROID)
    set_property(TARGET SolTranslator APPEND PROPERTY
            QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif ()

target_include_directories(SolTranslator PUBLIC src/)
target_link_libraries(
        SolTranslator PRIVATE
        Qt6::Widgets
        Qt6::Svg
        Qt6::SvgWidgets
        Qt6::Network
        Qt6::Concurrent
        Qt6::Core
        Qt6::Sql
        Qt6::Gui
)

# ============================
# QHotkey
add_library(qhotkey external/QHotkey/qhotkey.cpp)
add_library(QHotkey::QHotkey ALIAS qhotkey)

target_include_directories(qhotkey PUBLIC external/QHotkey)
target_link_libraries(qhotkey PUBLIC Qt6::Core Qt6::Gui)

if (APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    target_sources(qhotkey PRIVATE external/QHotkey/qhotkey_mac.cpp)
    target_link_libraries(qhotkey PRIVATE ${CARBON_LIBRARY})
elseif (WIN32)
    target_sources(qhotkey PRIVATE external/QHotkey/qhotkey_win.cpp)
else ()
    find_package(X11 REQUIRED)
    target_link_libraries(qhotkey PRIVATE ${X11_LIBRARIES})

    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE external/QHotkey/qhotkey_x11.cpp)
endif ()

target_link_libraries(SolTranslator PRIVATE QHotkey::QHotkey)



# ============================
# 현지화
##[[
qt_standard_project_setup(
        I18N_SOURCE_LANGUAGE ko
        I18N_TRANSLATED_LANGUAGES en ja
)
file(GLOB_RECURSE SOURCES_TO_TRANSLATE
        "src/*.cpp"
        "src/*.h"
)
qt_add_translations(
        SolTranslator
        TS_FILE_BASE sol
        TS_OUTPUT_DIRECTORY localization
        QM_FILES_OUTPUT_VARIABLE qm_output_files
        QM_OUTPUT_DIRECTORY translations
        MERGE_QT_TRANSLATIONS
        SOURCES ${SOURCES_TO_TRANSLATE}
)
install(FILES ${qm_output_files}
        DESTINATION translations)

add_dependencies(SolTranslator update_translations)
# ]]



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if (${Qt6_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SolTranslator)
endif ()
set_target_properties(SolTranslator PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)


# ============================
# deploy/install
include(GNUInstallDirs)
install(TARGETS SolTranslator
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# convert install path to an absolute path
# qt.conf requires an absolute path.
cmake_path(
        ABSOLUTE_PATH CMAKE_INSTALL_PREFIX
        NORMALIZE
        OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX
)

qt_generate_deploy_app_script(
        TARGET SolTranslator
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


qt_finalize_executable(SolTranslator)