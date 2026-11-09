project(platformthemeplugin)
include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets QuickControls2 DBus Gui)
find_package(Qt6GuiPrivate REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
find_package(Qt6WaylandClientPrivate REQUIRED)
find_package(KF6WindowSystem REQUIRED)
find_package(PkgConfig REQUIRED)

# qdbusmenubar uses them
remove_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)

pkg_check_modules(XCB_EWMH REQUIRED xcb xcb-ewmh x11)

set (SRCS
    main.cpp
    platformtheme.h
    platformtheme.cpp
    hintsettings.h
    hintsettings.cpp
    systemtrayicon.h
    systemtrayicon.cpp
    qdbusmenubar_p.h
    qdbusmenubar.cpp
    # Qt's D-Bus menu classes, bundled: Qt >= 6.10 no longer exports them
    dbusmenu/qdbusmenuadaptor_p.h
    dbusmenu/qdbusmenuadaptor.cpp
    dbusmenu/qdbusmenuregistrarproxy_p.h
    dbusmenu/qdbusmenuregistrarproxy.cpp
    dbusmenu/qdbusmenutypes_p.h
    dbusmenu/qdbusmenutypes.cpp
    dbusmenu/qdbusplatformmenu_p.h
    dbusmenu/qdbusplatformmenu.cpp
    x11integration.h
    x11integration.cpp
    waylandoutputorder.h
    waylandoutputorder.cpp
    statusnotifier/dbustypes.h
    statusnotifier/dbustypes.cpp
    statusnotifier/statusnotifieritem.h
    statusnotifier/statusnotifieritem.cpp
)

qt6_add_dbus_interface(SRCS org.kde.StatusNotifierWatcher.xml statusnotifierwatcher_interface)

qt6_add_dbus_adaptor(SRCS
    statusnotifier/org.kde.StatusNotifierItem.xml
    statusnotifier/statusnotifieritem.h
    StatusNotifierItem
)

add_library(lingmoplatformtheme MODULE ${SRCS})

# kde_output_order_v1 (the primary screen on Wayland): from plasma-wayland-protocols
# when installed, else the copy in protocols/
find_package(PlasmaWaylandProtocols QUIET)
if (PlasmaWaylandProtocols_FOUND)
    set(OUTPUT_ORDER_XML "${PLASMA_WAYLAND_PROTOCOLS_DIR}/kde-output-order-v1.xml")
else()
    set(OUTPUT_ORDER_XML "${CMAKE_CURRENT_SOURCE_DIR}/protocols/kde-output-order-v1.xml")
endif()
qt6_generate_wayland_protocol_client_sources(lingmoplatformtheme PRIVATE_CODE FILES "${OUTPUT_ORDER_XML}")

target_compile_definitions(lingmoplatformtheme
    PRIVATE
        "QT_NO_FOREACH"
        "LIB_FM_QT_SONAME=\"${LIB_FM_QT_SONAME}\""
)

target_include_directories(lingmoplatformtheme PRIVATE ${XCB_EWMH_INCLUDE_DIRS})

target_link_libraries(lingmoplatformtheme PRIVATE
    Qt6::GuiPrivate
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
    Qt6::Widgets
    Qt6::QuickControls2
    Qt6::Core
    Qt6::DBus
    KF6::WindowSystem
    ${XCB_EWMH_LIBRARIES}
)

# `qtpaths` on Arch is the Qt5 one; use the plugin dir Qt6 was built with
install(TARGETS lingmoplatformtheme LIBRARY DESTINATION "${QT6_INSTALL_PLUGINS}/platformthemes")
