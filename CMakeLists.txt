cmake_minimum_required(VERSION 3.16)
project(productivity_protector LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/widgets/desktop/systray")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(productivity_protector
    main.cpp
    window.cpp window.h
)

set_target_properties(productivity_protector PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(productivity_protector PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

# Resources:
set(productivity_protector_resource_files
    "images/bad.png"
    "images/heart.png"
    "images/trash.png"
)

qt_add_resources(productivity_protector "productivity_protector"
    PREFIX
        "/"
    FILES
        ${productivity_protector_resource_files}
)

install(TARGETS productivity_protector
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
