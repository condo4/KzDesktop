cmake_minimum_required(VERSION 3.16)

project(KzDesktop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Quick DBus WaylandCompositor REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)

find_package(KzXdg REQUIRED)
include_directories(${KZXDG_INCLUDE_DIR})

set(DBUS_APIS
    api/org.kazoe.desktop.applications.xml
)

set(PROJECT_QMLS
    qml/Main.qml
    qml/HomePage.qml
    qml/DesktopGridView.qml
    qml/StatusBar.qml
    qml/SystrayView.qml
    qml/Systray.qml
    qml/VKeyboard.qml
)

set(PROJECT_SOURCES
    src/main.cpp
    src/applicationmanager.h src/applicationmanager.cpp
    src/waylandentries.h src/waylandentries.cpp
    src/waylandentriesfiltered.h src/waylandentriesfiltered.cpp
    src/debug.cpp src/debug.h
)

set(OTHER_FILES
    conf/kzdesktop.sh
    conf/kzdesktop.service
    ${WAYLAND_PROTOCOLS}
    ${DBUS_APIS}
)

set(CLI_SOURCES
    tools/cli/main.cpp
)

qt_add_dbus_adaptor(ApplicationManager_SRCS api/org.kazoe.desktop.applications.xml src/applicationmanager.h ApplicationManager)
qt_add_dbus_interface(ApplicationManagerIface_SRCS api/org.kazoe.desktop.applications.xml ApplicationManagerIface)
qt_add_executable(kzdesktop ${PROJECT_SOURCES} ${PROJECT_QMLS}  ${OTHER_FILES} ${ApplicationManager_SRCS} ${APPMANAGER_SRCS})
qt_add_executable(desktopcli ${CLI_SOURCES} ${ApplicationManagerIface_SRCS} ${DBUS_APIS})
qt_add_qml_module(kzdesktop URI org.kazoe.desktop VERSION 1.0 QML_FILES ${PROJECT_QMLS} )

target_compile_definitions(kzdesktop PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(kzdesktop PRIVATE ${KZXDG_LIBRARIES} Qt6::Quick Qt6::DBus Qt6::WaylandCompositor systemd )
target_link_libraries(desktopcli PRIVATE Qt6::Core Qt6::DBus)


include(GNUInstallDirs)
install(TARGETS kzdesktop       DESTINATION ${CMAKE_INSTALL_BIN})
install(TARGETS desktopcli      DESTINATION ${CMAKE_INSTALL_BIN})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/conf/kzdesktop.service         DESTINATION /usr/lib/systemd/user)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/conf/kzdesktop.sh              DESTINATION /etc/profile.d/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/conf/kzdesktop.conf            DESTINATION /etc/system.conf.d/)
install(FILES ${DBUS_APIS}                                               DESTINATION /usr/share/dbus-1/interfaces/)
