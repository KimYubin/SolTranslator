cmake_minimum_required(VERSION 3.30.5)

project(SolTranslator VERSION 0.1.0 LANGUAGES CXX)


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

        src/SolTranslatorCore.cpp
        src/SolTranslatorCore.h

        resource/image/image.qrc
        resource/image/engines/engines.qrc
        resource/sql/sql.qrc
        resource/theme/theme.qrc

        src/EngineUnits/FinPoint/FinPointTrUnit.cpp
        src/EngineUnits/FinPoint/FinPointTrUnit.h
        src/EngineUnits/GoogleEngine/GoogleTrUnit.cpp
        src/EngineUnits/GoogleEngine/GoogleTrUnit.h
        src/EngineUnits/OpenAI/OpenAiTrUnit.cpp
        src/EngineUnits/OpenAI/OpenAiTrUnit.h

        src/EngineUnits/IAiEngine.cpp
        src/EngineUnits/IAiEngine.h
        src/EngineUnits/ITranslateEngine.cpp
        src/EngineUnits/ITranslateEngine.h
        src/EngineUnits/TranslateUnit.cpp
        src/EngineUnits/TranslateUnit.h

        src/Managers/AbstractManager.cpp
        src/Managers/AbstractManager.h

        src/Managers/ConfigManager.cpp
        src/Managers/ConfigManager.h
        src/Managers/DataManager.cpp
        src/Managers/DataManager.h
        src/Managers/DbWorker.cpp
        src/Managers/DbWorker.h
        src/Managers/EngineManager.cpp
        src/Managers/EngineManager.h
        src/Managers/GlobalHotKeyManager.cpp
        src/Managers/GlobalHotKeyManager.h
        src/Managers/HistoryManager.cpp
        src/Managers/HistoryManager.h
        src/Managers/InputSimulator_Impl_Linux.cpp
        src/Managers/InputSimulator_Impl_Mac.cpp
        src/Managers/InputSimulator_Impl_Win.cpp
        src/Managers/InputSimulator.cpp
        src/Managers/InputSimulator.h
        src/Managers/SecretStore.cpp
        src/Managers/SecretStore.h
        src/Managers/StyleManger.cpp
        src/Managers/StyleManger.h
        src/Managers/TranslateManager.cpp
        src/Managers/TranslateManager.h

        src/Settings/EngineOptionWidget.cpp
        src/Settings/EngineOptionWidget.h
        src/Settings/GeneralOptionWidget.cpp
        src/Settings/GeneralOptionWidget.h
        src/Settings/IOptionWidget.cpp
        src/Settings/IOptionWidget.h
        src/Settings/SettingsWidget.cpp
        src/Settings/SettingsWidget.h
        src/Settings/SettingsWidget.ui
        src/Settings/SettingWidgetFactory.cpp
        src/Settings/SettingWidgetFactory.h

        src/SubWidgets/DropdownMenu.cpp
        src/SubWidgets/DropdownMenu.h
        src/SubWidgets/EngineSelector.cpp
        src/SubWidgets/EngineSelector.h
        src/SubWidgets/ILoadingWidget.cpp
        src/SubWidgets/ILoadingWidget.h
        src/SubWidgets/KeySelectionList.cpp
        src/SubWidgets/KeySelectionList.h
        src/SubWidgets/LanguageSelector.cpp
        src/SubWidgets/LanguageSelector.h
        src/SubWidgets/LayoutTextEdit.cpp
        src/SubWidgets/LayoutTextEdit.h
        src/SubWidgets/LoadingBar.cpp
        src/SubWidgets/LoadingBar.h
        src/SubWidgets/LoadingSpinner.cpp
        src/SubWidgets/LoadingSpinner.h
        src/SubWidgets/OptionGroupBox.cpp
        src/SubWidgets/OptionGroupBox.h
        src/SubWidgets/OptionLineEdit.cpp
        src/SubWidgets/OptionLineEdit.h
        src/SubWidgets/ResultTextEdit.cpp
        src/SubWidgets/ResultTextEdit.h
        src/SubWidgets/SettingCard.cpp
        src/SubWidgets/SettingCard.h
        src/SubWidgets/SolBaseTextEdit.cpp
        src/SubWidgets/SolBaseTextEdit.h
        src/SubWidgets/SolButton.cpp
        src/SubWidgets/SolButton.h
        src/SubWidgets/SolContextMenuEventWidget.cpp
        src/SubWidgets/SolContextMenuEventWidget.h
        src/SubWidgets/SolSmoothAbstractScrollArea.hpp
        src/SubWidgets/SolSmoothComponent.cpp
        src/SubWidgets/SolSmoothComponent.h
        src/SubWidgets/SolSmoothScrollBar.cpp
        src/SubWidgets/SolSmoothScrollBar.h
        src/SubWidgets/SolToast.cpp
        src/SubWidgets/SolToast.h
        src/SubWidgets/SolToolTip.cpp
        src/SubWidgets/SolToolTip.h
        src/SubWidgets/SolToolTipBallon.cpp
        src/SubWidgets/SolToolTipBallon.h
        src/SubWidgets/SolTrayIcon.cpp
        src/SubWidgets/SolTrayIcon.h
        src/SubWidgets/SolWidgetFactory.cpp
        src/SubWidgets/SolWidgetFactory.h
        src/SubWidgets/SwitchButton.cpp
        src/SubWidgets/SwitchButton.h

        src/Support/WidgetInspector.cpp
        src/Support/WidgetInspector.h

        src/Types/EngineId.cpp
        src/Types/EngineId.h
        src/Types/ExJson.cpp
        src/Types/ExJson.h
        src/Types/OptionKey.cpp
        src/Types/OptionKey.h
        src/Types/OptionSpec.cpp
        src/Types/OptionSpec.h
        src/Types/SolConstants.cpp
        src/Types/SolConstants.h
        src/Types/SolDatabase.cpp
        src/Types/SolDatabase.h
        src/Types/SolExpected.hpp
        src/Types/SolGuard.cpp
        src/Types/SolGuard.h
        src/Types/SolHashQueue.hpp
        src/Types/SolSharedGuard.cpp
        src/Types/SolSharedGuard.h
        src/Types/SolTypes.cpp
        src/Types/SolTypes.h
        src/Types/TextCacheKey.hpp
        src/Types/ToolTipData.h
        src/Types/ToolTipData.cpp
        src/Types/TranslateRequest.cpp
        src/Types/TranslateRequest.h

        src/Utils/EnumUtils.hpp
        src/Utils/SolAsync.hpp
        src/Utils/SolChrono.cpp
        src/Utils/SolChrono.h
        src/Utils/SolDocument.cpp
        src/Utils/SolDocument.h
        src/Utils/SolI18n.cpp
        src/Utils/SolI18n.h
        src/Utils/SolLog.cpp
        src/Utils/SolLog.h
        src/Utils/SolLogWorker.cpp
        src/Utils/SolLogWorker.h
        src/Utils/SolPath.cpp
        src/Utils/SolPath.h
        src/Utils/SolSingleApplication.cpp
        src/Utils/SolSingleApplication.h
        src/Utils/SolTextSpliter.cpp
        src/Utils/SolTextSpliter.h
        src/Utils/SolTextTable.cpp
        src/Utils/SolTextTable.h
        src/Utils/SolUtilibrary.cpp
        src/Utils/SolUtilibrary.h
        src/Utils/UniqueQObjectPtr.hpp

        src/Widgets/HistoryCacheData.cpp
        src/Widgets/HistoryCacheData.h
        src/Widgets/HistoryListDelegate.cpp
        src/Widgets/HistoryListDelegate.h
        src/Widgets/HistoryListView.cpp
        src/Widgets/HistoryListView.h
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

)

# ============================
# appicon windows
if (WIN32)
    set(app_icon_resource_windows "resource/image/icon/SolTranslator.rc")
    target_sources(SolTranslator PRIVATE ${app_icon_resource_windows})
endif ()

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
# Magic Enum thirdparty/magic_enum.hpp
add_library(magic_enum INTERFACE)
add_library(magic_enum::magic_enum ALIAS magic_enum)

target_include_directories(magic_enum INTERFACE thirdparty/magic_enum)
target_link_libraries(SolTranslator PRIVATE magic_enum::magic_enum)

# ============================
# MurmurHash thirdparty/MurmurHash/MurmurHash3.h
add_library(MurmurHash STATIC thirdparty/MurmurHash/MurmurHash3.cpp)
add_library(MurmurHash::MurmurHash ALIAS MurmurHash)

target_include_directories(MurmurHash PUBLIC thirdparty/MurmurHash)
target_link_libraries(SolTranslator PRIVATE MurmurHash::MurmurHash)

# ============================
# QHotkey
add_library(qhotkey thirdparty/QHotkey/qhotkey.cpp)
add_library(QHotkey::QHotkey ALIAS qhotkey)

target_include_directories(qhotkey PUBLIC thirdparty/QHotkey)
target_link_libraries(qhotkey PUBLIC Qt6::Core Qt6::Gui)

if (APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    target_sources(qhotkey PRIVATE thirdparty/QHotkey/qhotkey_mac.cpp)
    target_link_libraries(qhotkey PRIVATE ${CARBON_LIBRARY})
elseif (WIN32)
    target_sources(qhotkey PRIVATE thirdparty/QHotkey/qhotkey_win.cpp)
else ()
    find_package(X11 REQUIRED)
    target_link_libraries(qhotkey PRIVATE ${X11_LIBRARIES})

    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE thirdparty/QHotkey/qhotkey_x11.cpp)
endif ()

target_link_libraries(SolTranslator PRIVATE QHotkey::QHotkey)


# ============================
# QtCustom thirdparty/md4c
add_subdirectory(thirdparty/md4c)
target_link_libraries(SolTranslator PRIVATE md4c)

# ============================
# QtCustom thirdparty/QtCustom/QtCustom3.h
add_library(
        QtCustom
        STATIC
        thirdparty/QtCustom/SolMarkdownWriter.cpp
        thirdparty/QtCustom/SolMarkdownImporter.cpp
)
add_library(QtCustom::QtCustom ALIAS QtCustom)

target_include_directories(QtCustom PUBLIC thirdparty/QtCustom)
target_link_libraries(QtCustom PUBLIC Qt6::Core Qt6::Gui)

target_link_libraries(SolTranslator PRIVATE QtCustom::QtCustom)

# ============================
# qtkeychain thirdparty/qtkeychain
add_subdirectory(thirdparty/qtkeychain)
target_link_libraries(SolTranslator PRIVATE qtkeychain)

# ============================
# Internationalization
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

# Convert install path to an absolute path
# qt.conf requires an absolute path.
cmake_path(
        ABSOLUTE_PATH CMAKE_INSTALL_PREFIX
        NORMALIZE
        OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX
)

if (WIN32)
    qt_generate_deploy_app_script(
            TARGET SolTranslator
            OUTPUT_SCRIPT deploy_script
            NO_COMPILER_RUNTIME
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})

    # Redistributable windows runtime DLLs.
    # NO_COMPILER_RUNTIME prevents the inclusion of vc_redist.x64.exe.
    include(InstallRequiredSystemLibraries)
else ()
    qt_generate_deploy_app_script(
            TARGET SolTranslator
            OUTPUT_SCRIPT deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif ()


qt_finalize_executable(SolTranslator)