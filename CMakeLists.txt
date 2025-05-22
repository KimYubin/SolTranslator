cmake_minimum_required(VERSION 3.16)

project(FinTranslator VERSION 0.1 LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR}  REQUIRED COMPONENTS Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Concurrent)

set(PROJECT_SOURCES
        src/main.cpp

        src/FinTranslatorCore.cpp
        src/FinTranslatorCore.h
        src/Widgets/FinTranslatorMainWidget.cpp
        src/Widgets/FinTranslatorMainWidget.h
        src/Widgets/FinTranslatorMainWidget.ui

        src/TranslateManager.cpp
        src/TranslateManager.h
        src/TranslateUnit.cpp
        src/TranslateUnit.h
        src/ConfigManager.cpp
        src/ConfigManager.h
        src/FinTypes.cpp
        src/FinTypes.h
        src/magic_enum.hpp

        src/GlobalHotKeyManager.cpp
        src/GlobalHotKeyManager.h
        src/RunCopKey.cpp
        src/RunCopKey.h
        src/RunCopy_Impl_Win.cpp
        src/RunCopy_Impl_Linux.cpp
        src/RunCopy_Impl_Mac.cpp
        src/FinHashQueue.cpp
        src/FinHashQueue.h
        src/TlUnitFactory.cpp
        src/TlUnitFactory.h
        src/TranslateUnitOpenAi.cpp
        src/TranslateUnitOpenAi.h
        src/TranslateUnitGoogle.cpp
        src/TranslateUnitGoogle.h
        src/DataManager.cpp
        src/DataManager.h
        src/AbstractManager.cpp
        src/AbstractManager.h
        src/Widgets/SimpleTranslatePopup.cpp
        src/Widgets/SimpleTranslatePopup.h
        src/Widgets/SimpleTranslatePopup.ui
        src/Widgets/TextEditTranslateWidget.cpp
        src/Widgets/TextEditTranslateWidget.h
        src/Widgets/TextEditTranslateWidget.ui
        src/AsyncManager.cpp
        src/AsyncManager.h
        src/Settings/SettingsWidget.cpp
        src/Settings/SettingsWidget.h
        src/Settings/SettingsWidget.ui
        src/Settings/IOptionWidget.cpp
        src/Settings/IOptionWidget.h
        src/Settings/EngineOptionWidget.cpp
        src/Settings/EngineOptionWidget.h
        src/Settings/GeneralOptionWidget.cpp
        src/Settings/GeneralOptionWidget.h
        src/Settings/GeneralOptionWidget.ui
        src/FinConstants.cpp
        src/FinConstants.h

)

add_library(qhotkey src/QHotkey/qhotkey.cpp)
add_library(QHotkey::QHotkey ALIAS qhotkey)


if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FinTranslator
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}

            resource/theme/theme.qrc
            resource/image/image.qrc
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET FinTranslator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(FinTranslator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(FinTranslator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

################
# Your-external "mylib", add GLOBAL if the imported library is located in directories above the current.
# 외부 "mylib"를 추가하시오. 현재 디렉토리 위의 디렉토리에 있는 경우 GLOBAL을 추가하십시오.
add_library( Qt SHARED IMPORTED )
# You can define two import-locations: one for debug and one for release.
# 디버그용과 릴리스용으로 두 개의 가져오기 위치를 정의할 수 있습니다.
set_target_properties( Qt PROPERTIES IMPORTED_LOCATION ${QTDIR}/bin )
################


target_link_libraries(FinTranslator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(FinTranslator PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(FinTranslator PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)


target_include_directories(qhotkey PUBLIC src/QHotkey)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

target_link_libraries(FinTranslator PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(qhotkey PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)

if(APPLE)
    find_library(CARBON_LIBRARY Carbon)
    mark_as_advanced(CARBON_LIBRARY)

    target_sources(qhotkey PRIVATE src/QHotkey/qhotkey_mac.cpp)
    target_link_libraries(qhotkey PRIVATE ${CARBON_LIBRARY})
elseif(WIN32)
    target_sources(qhotkey PRIVATE src/QHotkey/qhotkey_win.cpp)
else()
    find_package(X11 REQUIRED)
    target_link_libraries(qhotkey PRIVATE ${X11_LIBRARIES})
    
    include_directories(${X11_INCLUDE_DIR})
    target_sources(qhotkey PRIVATE src/QHotkey/qhotkey_x11.cpp)
endif()

target_link_libraries(FinTranslator PRIVATE QHotkey::QHotkey)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.FinTranslator)
endif()
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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FinTranslator)
endif()
