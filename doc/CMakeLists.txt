add_subdirectory(calligra)
if(SHOULD_BUILD_KEXI)
    add_subdirectory(kexi)
endif(SHOULD_BUILD_KEXI)
if(SHOULD_BUILD_STAGE)
    add_subdirectory(stage)
endif(SHOULD_BUILD_STAGE)
if(SHOULD_BUILD_SHEETS)
    add_subdirectory(sheets)
endif(SHOULD_BUILD_SHEETS)
INSTALL(FILES   calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
