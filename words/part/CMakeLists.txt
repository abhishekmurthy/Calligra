project(wordspart)

add_subdirectory( tests )

include_directories( ${KOTEXT_INCLUDES}  ${KDE4_INCLUDE_DIR}/threadweaver/ )

########### KWordPrivate library ###############

set(wordsprivate_LIB_SRCS
    KWFactory.cpp
    KWord.cpp
    KWApplicationConfig.cpp
    KWCanvasBase.cpp
    KWCanvas.cpp
    KWCanvasItem.cpp
    KWDocument.cpp
    KWGui.cpp
    KWView.cpp
    KWPage.cpp
    KWPageManager.cpp
    KWPageStyle.cpp
    KWPageTextInfo.cpp
    KWViewMode.cpp
    KWViewModeNormal.cpp
    KWViewModePreview.cpp
    KWStatusBar.cpp
    KWOdfLoader.cpp
    KWOdfWriter.cpp
    KWOdfSharedLoadingData.cpp
    KWDLoader.cpp

    frames/KWTextDocumentLayout.cpp
    frames/KWFrameLayout.cpp
    frames/KWFrame.cpp
    frames/KWFrameSet.cpp
    frames/KWTextFrame.cpp
    frames/KWTextFrameSet.cpp
    frames/KWCopyShape.cpp
    frames/KWAnchorStrategy.cpp
    frames/KWOutlineShape.cpp
    frames/Outline.cpp
    frames/KWPageBackground.cpp

    dialogs/KWFrameConnectSelector.cpp
    dialogs/KWFrameGeometry.cpp
    dialogs/KWFrameRunaroundProperties.cpp
    dialogs/KWGeneralFrameProperties.cpp
    dialogs/KWFrameDialog.cpp
    dialogs/KWShapeConfigFactory.cpp
    dialogs/KWDocumentColumns.cpp
    dialogs/KWStartupWidget.cpp
    dialogs/KWPageSettingsDialog.cpp
    dialogs/KWStatistics.cpp
    dialogs/KWStatisticsDialog.cpp
    dialogs/KWPrintingDialog.cpp
    dialogs/KWCreateBookmarkDialog.cpp
    dialogs/KWSelectBookmarkDialog.cpp
    dialogs/KWInsertPageDialog.cpp
    dialogs/KWInsertInlineNoteDialog.cpp
    dockers/KWStatisticsDocker.cpp

    commands/KWPageStylePropertiesCommand.cpp
    commands/KWPageInsertCommand.cpp
    commands/KWPageRemoveCommand.cpp
    commands/KWNewPageStyleCommand.cpp
    commands/KWFrameDeleteCommand.cpp
    commands/KWFrameCreateCommand.cpp
    commands/KWCreateOutlineCommand.cpp
    commands/KWClipFrameCommand.cpp
    commands/KWRemoveFrameClipCommand.cpp
    commands/KWChangePageStyleCommand.cpp
)
if( Soprano_FOUND )
    set(wordsprivate_LIB_SRCS ${wordsprivate_LIB_SRCS}
        dockers/KWRdfDocker.cpp
        dockers/KWRdfDockerFactory.cpp
        dockers/KWRdfDockerTree.cpp
    )
endif( Soprano_FOUND )

kde4_add_ui_files(wordsprivate_LIB_SRCS
    dialogs/KWFrameConnectSelector.ui
    dialogs/KWFrameGeometry.ui
    dialogs/KWFrameRunaroundProperties.ui
    dialogs/KWGeneralFrameProperties.ui
    dialogs/KWDocumentColumns.ui
    dialogs/KWStartupWidget.ui
    dialogs/KWStatistics.ui
    dialogs/KWCreateBookmark.ui
    dialogs/KWSelectBookmark.ui
    dialogs/KWInsertPageDialog.ui
    dialogs/KWInsertInlineNoteDialog.ui
    dockers/KWStatisticsDocker.ui
)
if( Soprano_FOUND )
    kde4_add_ui_files(wordsprivate_LIB_SRCS
        dockers/KWRdfDocker.ui
    )
endif( Soprano_FOUND )


kde4_add_library(wordsprivate SHARED ${wordsprivate_LIB_SRCS})

target_link_libraries(wordsprivate komain)
if( Soprano_FOUND )
    target_link_libraries(wordsprivate ${SOPRANO_LIBRARIES})
endif( Soprano_FOUND )
target_link_libraries(wordsprivate LINK_INTERFACE_LIBRARIES komain)

set_target_properties(wordsprivate
    PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
)
install(TARGETS wordsprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### KWordPart Library ###############

set(wordspart_PART_SRCS KWFactoryInit.cpp )


kde4_add_plugin(wordspart ${wordspart_PART_SRCS})



target_link_libraries(wordspart ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} wordsprivate )

install(TARGETS wordspart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### KWord exe ###############

set(words_KDEINIT_SRCS main.cpp )

kde4_add_app_icon(words_KDEINIT_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/words.png")

kde4_add_kdeinit_executable( words ${words_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(words PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(words PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.words")
   set_target_properties(words PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Words")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_words  ${KDE4_KDEUI_LIBS} komain )

install(TARGETS kdeinit_words  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( words kdeinit_words )
install(TARGETS words  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES words.rc words_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/words)
install( FILES wordspart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS words.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES wordsrc DESTINATION ${CONFIG_INSTALL_DIR} )

install( FILES
kword_export.h
KWPage.h
KWPageManager.h
KWPageStyle.h
KWord.h
KWCanvasBase.h
KWCanvas.h
KWCanvasItem.h
KWDocument.h
KWApplicationConfig.h
frames/KWFrameLayout.h
KWViewMode.h
DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)
