cmake_minimum_required(VERSION 3.16)
project(furkan VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Multimedia)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(furkan WIN32 MACOSX_BUNDLE
    furkan.cpp furkan.h furkan.ui
    main.cpp
)
target_link_libraries(furkan PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Multimedia
)


# Resources:
set(images_resource_files
    "images/bilgi.png"
    "images/dinlet.png"
    "images/duraklat.png"
    "images/furkan.ico"
    "images/furkan.png"
    "images/geri.png"
    "images/ileri.png"
    "images/iptal.png"
)

qt_add_resources(furkan "images"
    PREFIX
        "/"
    FILES
        ${images_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(furkan PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS furkan
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET furkan
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
