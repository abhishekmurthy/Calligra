if(SHOULD_BUILD_PLUGIN_GOOGLEDOCS)
    add_subdirectory(googledocs)
endif(SHOULD_BUILD_PLUGIN_GOOGLEDOCS)

if (SHOULD_BUILD_PLUGIN_THREEDSHAPE)
    add_subdirectory(threedshape)
endif (SHOULD_BUILD_PLUGIN_THREEDSHAPE)

if(SHOULD_BUILD_PLUGIN_SHEETS_PIVOTTABLES)
    add_subdirectory(pivottables)
endif(SHOULD_BUILD_PLUGIN_SHEETS_PIVOTTABLES)
