ecm_add_app_icon(SNORENOTIFY_SETTINGS_DEPS ICONS ${PROJECT_SOURCE_DIR}/data/128-apps-snore.png)

qt5_wrap_ui(UI settingswindow.ui)

add_executable( snoresettings WIN32 main.cpp settingswindow.cpp ${UI} ${SNORENOTIFY_SETTINGS_DEPS})
target_link_libraries( snoresettings Snore::Libsnore )
install(TARGETS snoresettings ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

if(UNIX)
     configure_file("${CMAKE_CURRENT_SOURCE_DIR}/snoresettings.desktop.in" "${CMAKE_CURRENT_BINARY_DIR}/snoresettings.desktop" @ONLY)
     install(FILES "${CMAKE_CURRENT_BINARY_DIR}/snoresettings.desktop" DESTINATION ${KDE_INSTALL_APPDIR})
endif()

