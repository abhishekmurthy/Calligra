add_definitions(-DKDE_DEFAULT_DEBUG_AREA=30527)

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/filters/libmsooxml )

########### next target ###############

set(xlsximport_PART_SRCS
    XlsxImport.cpp
    XlsxXmlCommonReader.cpp
    XlsxXmlDocumentReader.cpp
    XlsxXmlWorksheetReader.cpp
    XlsxXmlSharedStringsReader.cpp
    XlsxXmlStylesReader.cpp
    XlsxXmlDrawingReader.cpp
    XlsxXmlChartReader.cpp
    XlsxXmlCommentsReader.cpp
    XlsxXmlTableReader.cpp
    NumberFormatParser.cpp
    ChartExport.cpp
    FormulaParser.cpp
)

kde4_add_plugin(xlsximport ${xlsximport_PART_SRCS})

target_link_libraries(
    xlsximport
    msooxml
    koodf
    komain
    calligrasheetscommon
)

install(TARGETS xlsximport DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES
  kspread_xlsx_import.desktop
#  kspread_xlsx_export.desktop
  DESTINATION ${SERVICES_INSTALL_DIR}
)

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES sheets_xlsx_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)

########## unit tests ###################

set(TestFormulaParser_SRCS
    FormulaParser.cpp
    TestFormulaParser.cpp
)

kde4_add_unit_test(TestFormulaParser TESTNAME sheets-xlsx-FormulaParser ${TestFormulaParser_SRCS})
target_link_libraries(TestFormulaParser msooxml calligrasheetscommon ${KDE4_KDEUI_LIBS} ${QT_QTTEST_LIBRARY})
