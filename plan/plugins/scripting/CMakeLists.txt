
include_directories( 
    ${CMAKE_SOURCE_DIR}/plan
    ${KPLATO_INCLUDES}
    
    ${KOKROSS_INCLUDES}
)

add_subdirectory( scripts )
add_subdirectory( tests )

set(krossmodulekplato_PART_SRCS
    Account.cpp
    Calendar.cpp
    Node.cpp
    Resource.cpp
    ResourceGroup.cpp
    Schedule.cpp
    Project.cpp
    Module.cpp
    ScriptingPart.cpp
    ScriptingWidgets.cpp
)

kde4_add_ui_files(krossmodulekplato_PART_SRCS
    ScriptingDataQueryView.ui
)


qt4_wrap_cpp(krossmodulekplato_PART_SRCS
)

kde4_add_plugin(krossmodulekplato ${krossmodulekplato_PART_SRCS})
target_link_libraries(
    krossmodulekplato
    kokross
    planprivate
    kplatokernel
    kplatomodels
    komain
)

install(TARGETS krossmodulekplato DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES krossmoduleplan.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES scripting.rc DESTINATION ${DATA_INSTALL_DIR}/plan/kpartplugins)