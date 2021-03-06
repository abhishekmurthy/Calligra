include_directories( 
        ${CMAKE_SOURCE_DIR}/sheets
        ${CMAKE_SOURCE_DIR}/sheets/commands
        ${CMAKE_SOURCE_DIR}/sheets/ui
	${KOTEXT_INCLUDES} 
	${KOMAIN_INCLUDES} 
	${KOKROSS_INCLUDES}
	)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")

add_subdirectory( scripts )

# the Sheets Scripting KParts::Plugin + Kross module
set(krossmodulesheets_PART_SRCS ScriptingModule.cpp ScriptingFunction.cpp ScriptingWidgets.cpp ScriptingPart.cpp ScriptingListener.cpp)

qt4_wrap_cpp(krossmodulesheets_PART_SRCS ScriptingReader.h ScriptingWriter.h)
kde4_add_plugin(krossmodulesheets ${krossmodulesheets_PART_SRCS})

target_link_libraries(krossmodulesheets ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} calligrasheetscommon kokross)
install(TARGETS krossmodulesheets DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

#install( FILES  sheetsscripting.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krossmodulesheets.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  scripting.rc  DESTINATION ${DATA_INSTALL_DIR}/sheets/kpartplugins)
