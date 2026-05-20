include(GenerateExportHeader)

set(QTKEYCHAIN_TARGET_NAME qtkeychain)

list(APPEND qtkeychain_LIBRARIES Qt6::Core)
set(qtkeychain_SOURCES
        keychain.cpp
        qkeychain_export.h
        keychain.h
)

if (WIN32)
    option(USE_CREDENTIAL_STORE "Build with windows CredentialStore support" ON)
    if (USE_CREDENTIAL_STORE)
        add_definitions(-DUSE_CREDENTIAL_STORE=1)
    endif ()

    list(APPEND qtkeychain_SOURCES keychain_win.cpp)
    list(APPEND qtkeychain_LIBRARIES crypt32)
    if (NOT USE_CREDENTIAL_STORE)
        list(APPEND qtkeychain_SOURCES plaintextstore.cpp)
    endif ()

endif ()

if (APPLE)
    list(APPEND qtkeychain_SOURCES keychain_apple.mm)
    list(APPEND qtkeychain_LIBRARIES "-framework Foundation" "-framework Security")
endif ()

if (HAIKU)
    list(APPEND qtkeychain_SOURCES keychain_haiku.cpp)

    find_library(BE_LIBRARY be REQUIRED)
    list(APPEND qtkeychain_LIBRARIES ${BE_LIBRARY})
endif ()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT HAIKU AND NOT EMSCRIPTEN)
    option(LIBSECRET_SUPPORT "Build with libsecret support" ON)

    if (LIBSECRET_SUPPORT)
        pkg_check_modules(LIBSECRET REQUIRED libsecret-1)
        list(APPEND qtkeychain_LIBRARIES_PRIVATE ${LIBSECRET_LIBRARIES})
    endif ()

    list(APPEND qtkeychain_SOURCES keychain_unix.cpp gnomekeyring.cpp libsecret.cpp plaintextstore.cpp)
    qt_add_dbus_interface(qtkeychain_SOURCES org.kde.KWallet.xml kwallet_interface KWalletInterface)
    list(APPEND qtkeychain_LIBRARIES ${QTDBUS_LIBRARIES})
endif ()

if (ANDROID)
    list(APPEND qtkeychain_SOURCES keychain_android.cpp androidkeystore.cpp plaintextstore.cpp)
    list(APPEND qtkeychain_LIBRARIES_PRIVATE ${QTANDROIDEXTRAS_LIBRARIES})
endif ()


add_library(${QTKEYCHAIN_TARGET_NAME} ${qtkeychain_SOURCES} ${qtkeychain_MOC_OUTFILES} ${qtkeychain_QM_FILES})
if (WIN32)
    set_target_properties(${QTKEYCHAIN_TARGET_NAME} PROPERTIES DEBUG_POSTFIX "d")
endif ()

target_link_libraries(${QTKEYCHAIN_TARGET_NAME} PUBLIC ${qtkeychain_LIBRARIES} PRIVATE ${qtkeychain_LIBRARIES_PRIVATE})


if (MSVC)
    target_compile_options(${QTKEYCHAIN_TARGET_NAME} PRIVATE /W4 /utf-8)
else ()
    target_compile_options(${QTKEYCHAIN_TARGET_NAME} PRIVATE -Wall -Werror=return-type)
endif ()

if (LIBSECRET_SUPPORT)
    target_compile_definitions(${QTKEYCHAIN_TARGET_NAME} PRIVATE HAVE_LIBSECRET=1)
endif ()

if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT HAIKU AND NOT EMSCRIPTEN)
    target_compile_definitions(${QTKEYCHAIN_TARGET_NAME} PRIVATE KEYCHAIN_DBUS=1)
endif ()


if (NOT INTERFACE_INCLUDE_SUFFIX)
    set(INTERFACE_INCLUDE_SUFFIX include)
endif ()
# Where to find includes when building the library and using it uninstalled: in the parent dir, so that the code has to use <qtkeychain/keychain.h>
target_include_directories(${QTKEYCHAIN_TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

generate_export_header(${QTKEYCHAIN_TARGET_NAME}
        EXPORT_FILE_NAME qkeychain_export.h
        EXPORT_MACRO_NAME QKEYCHAIN_EXPORT
)


if (NOT APPLE)
    set_target_properties(${QTKEYCHAIN_TARGET_NAME} PROPERTIES
            INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
    )
endif ()
