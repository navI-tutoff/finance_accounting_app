cmake_minimum_required(VERSION 3.16)

project(finance_accounting_application VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Network Charts PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network Charts PrintSupport)

set(PROJECT_SOURCES
        main.cpp

        widgets/authorization_window.cpp
        widgets/authorization_window.h
        widgets/authorization_window.ui

        widgets/major_application_window.cpp
        widgets/major_application_window.h
        widgets/major_application_window.ui

        widgets/main_stacked_widget.h
        widgets/main_stacked_widget.cpp
        widgets/main_stacked_widget.ui

        widgets/crypto_stacked_widget.h
        widgets/crypto_stacked_widget.cpp
        widgets/crypto_stacked_widget.ui

        widgets/interaction_with_cryptocoin_dialog.h
        widgets/interaction_with_cryptocoin_dialog.cpp
        widgets/interaction_with_cryptocoin_dialog.ui

        data_structures/database.h
        data_structures/database.cpp

        data_structures/user_session.h
        data_structures/user_session.cpp

        resource.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(finance_accounting_application
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resource.qrc

        widgets/authorization_window.cpp
        widgets/authorization_window.h
        widgets/authorization_window.ui

        widgets/major_application_window.cpp
        widgets/major_application_window.h
        widgets/major_application_window.ui

        widgets/main_stacked_widget.h
        widgets/main_stacked_widget.cpp
        widgets/main_stacked_widget.ui

        widgets/crypto_stacked_widget.h
        widgets/crypto_stacked_widget.cpp
        widgets/crypto_stacked_widget.ui

        widgets/interaction_with_cryptocoin_dialog.h
        widgets/interaction_with_cryptocoin_dialog.cpp
        widgets/interaction_with_cryptocoin_dialog.ui

        data_structures/database.h
        data_structures/database.cpp

        data_structures/user_session.h
        data_structures/user_session.cpp

        notes.md

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET finance_accounting_application APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(finance_accounting_application SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(finance_accounting_application
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(finance_accounting_application PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Sql Qt${QT_VERSION_MAJOR}::Network Qt6::Charts Qt6::PrintSupport)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.finance_accounting_application)
endif()
set_target_properties(finance_accounting_application PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS finance_accounting_application
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(finance_accounting_application)
endif()
