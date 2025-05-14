cmake_minimum_required(VERSION 3.16)

project(EventCalendar VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Keresd meg a szükséges Qt modulokat: Widgets és Sql
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    topbar.cpp
    topbar.h
    leftsidebar.cpp
    leftsidebar.h
    myavailability.cpp
    myavailability.h
    weekview.cpp
    weekview.h
    eventmodel.cpp
    eventmodel.h
    databasemanager.cpp
    databasemanager.h
    event.cpp
    event.h
    eventtype.h
    addeventdialog.cpp
    addeventdialog.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EventCalendar
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

    )
else()
    add_executable(EventCalendar
        ${PROJECT_SOURCES}
    )
endif()

# Linkeljük mind a Widgets, mind a Sql modult
target_link_libraries(EventCalendar
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

set_target_properties(EventCalendar PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS EventCalendar
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EventCalendar)
endif()
