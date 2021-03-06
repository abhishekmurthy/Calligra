include_directories(${TEXTLAYOUT_INCLUDES})

add_subdirectory( tests )

########### next target ###############
set(textlayout_LIB_SRCS
    KoTextLayoutCellHelper.cpp
    KoTextLayoutArea.cpp
    KoTextLayoutArea_paint.cpp
    KoTextLayoutEndNotesArea.cpp
    KoTextLayoutTableArea.cpp
    KoTextLayoutNoteArea.cpp
    KoTextLayoutRootArea.cpp
    KoTextLayoutRootAreaProvider.cpp
    KoTextDocumentLayout.cpp
    ListItemsHelper.cpp
    KoTextShapeContainerModel.cpp
    RunAroundHelper.cpp
    KoTextLayoutObstruction.cpp
    FrameIterator.cpp
    TableIterator.cpp
    KoPointedAt.cpp
    KoTextShapeData.cpp
    FloatingAnchorStrategy.cpp
    InlineAnchorStrategy.cpp
    AnchorStrategy.cpp
    ToCGenerator.cpp
    DummyDocumentLayout.cpp
    IndexGeneratorManager.cpp
    KoStyleThumbnailer.cpp
)


kde4_add_library(textlayout SHARED ${textlayout_LIB_SRCS})

target_link_libraries(textlayout kotext)
target_link_libraries(textlayout LINK_INTERFACE_LIBRARIES kotext)

set_target_properties(textlayout PROPERTIES
   VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS textlayout  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

#install( FILES ttt.tt DESTINATION ${SERVICETYPES_INSTALL_DIR})

install(
    FILES
    KoTextDocumentLayout.h
    KoTextLayoutArea.h
    KoTextLayoutRootArea.h
    KoTextShapeData.h
    KoPointedAt.h
    KoStyleThumbnailer.h
    textlayout_export.h
    DESTINATION
    ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
