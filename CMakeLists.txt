cmake_minimum_required(VERSION 3.16)

project(FinTranslator VERSION 0.1 LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5)

find_package(Qt6 REQUIRED COMPONENTS
        Widgets
        Svg
        SvgWidgets
        Network
        Concurrent
        Core
        Gui
        LinguistTools
)

file(GLOB_RECURSE PROJECT_SOURCES
        "src/*.cpp"
        "src/*.h"
        "src/*.ui"
        "src/*.qml"
        "resource/*.qrc"
)


qt_add_executable(FinTranslator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        external/magic_enum.hpp
        src/Widgets/PopupTranslateWidget.cpp
        src/Widgets/PopupTranslateWidget.h
)

if (WIN32)
    #! [appicon_windows]
    set(app_icon_resource_windows "resource/image/icon/FinTranslator.rc")
    target_sources(FinTranslator PRIVATE ${app_icon_resource_windows})
    #! [appicon_windows]
endif ()

# Android 환경일 경우에만 패키지 소스 디렉토리 설정
# Define target properties for Android with Qt 6 as:
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
if (ANDROID)
    set_property(TARGET FinTranslator APPEND PROPERTY
            QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif ()

target_include_directories(FinTranslator PUBLIC src/)
target_link_libraries(FinTranslator PRIVATE
        Qt6::Widgets
        Qt6::Svg
        Qt6::SvgWidgets
        Qt6::Network
        Qt6::Concurrent
        Qt6::Core
        Qt6::Gui
)


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

target_link_libraries(FinTranslator PRIVATE QHotkey::QHotkey)



##[[
#### 현지화
qt_standard_project_setup(
        I18N_SOURCE_LANGUAGE ko
        I18N_TRANSLATED_LANGUAGES en ja
)
file(GLOB_RECURSE SOURCES_TO_TRANSLATE
        "src/*.cpp"
        "src/*.h"
)
qt_add_translations(FinTranslator
        TS_FILE_BASE fin
        TS_OUTPUT_DIRECTORY localization
        QM_OUTPUT_DIRECTORY translations
        MERGE_QT_TRANSLATIONS
        SOURCES ${SOURCES_TO_TRANSLATE}
)

add_dependencies(FinTranslator update_translations)
# ]]



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if (${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FinTranslator)
endif ()
set_target_properties(FinTranslator PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FinTranslator
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FinTranslator)
endif ()
