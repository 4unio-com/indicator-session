include (GdbusCodegen)

set(BACKEND_GENERATED_SOURCES
)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-display-manager
                   org.freedesktop
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.DisplayManager.Seat.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-webcredentials
                   com.canonical.indicators
                   ${CMAKE_CURRENT_SOURCE_DIR}/com.canonical.indicators.webcredentials.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-accounts
                   org.freedesktop
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.Accounts.xml)
                   
add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-user
                   org.freedesktop
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.Accounts.User.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-login1-manager
                   org.freedesktop
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.login1.Manager.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-login1-seat
                   org.freedesktop
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.login1.Seat.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-login1-user
                   org.freedesktop
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.freedesktop.login1.User.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES gnome-screen-saver
                   org
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.gnome.ScreenSaver.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES gnome-session-manager
                   org
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.gnome.SessionManager.xml)

add_gdbus_codegen (BACKEND_GENERATED_SOURCES dbus-end-session-dialog
                   org.gnome.SessionManager
                   ${CMAKE_CURRENT_SOURCE_DIR}/org.gnome.SessionManager.EndSessionDialog.xml)

set (SOURCES actions.c guest.c users.c backend-dbus.c utils.c)

# add warnings/coverage info on handwritten files
# but not the autogenerated ones...
set_source_files_properties (${SOURCES}
                             PROPERTIES COMPILE_FLAGS " -g ${CC_WARNING_ARGS} ${GCOV_FLAGS}")

# add the bin dir to our include path s.t. our code can find the autogenerated header files
include_directories (${CMAKE_CURRENT_BINARY_DIR} ${SERVICE_INCLUDE_DIRS})

add_library (backenddbus STATIC ${SOURCES} ${BACKEND_GENERATED_SOURCES})
