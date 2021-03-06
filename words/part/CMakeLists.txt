project(wordspart)

#TODO port to textlayout-rework
add_subdirectory( tests )

include_directories( ${KORDF_INCLUDES} ${KOTEXT_INCLUDES} ${TEXTLAYOUT_INCLUDES} ${KDE4_INCLUDE_DIR}/threadweaver/)

########### WordsPrivate library ###############

set(wordsprivate_LIB_SRCS
    KWFactory.cpp
    Words.cpp
    KWApplicationConfig.cpp
    KWCanvasBase.cpp
    KWCanvas.cpp
    KWCanvasItem.cpp
    KWDocument.cpp
    KWGui.cpp
    KWView.cpp
    KWPart.cpp
    KWPage.cpp
    KWPageCacheManager.cpp
    KWPageManager.cpp
    KWPageStyle.cpp
    KWViewMode.cpp
    KWViewModeNormal.cpp
    KWViewModePreview.cpp
    KWStatusBar.cpp
    KWOdfLoader.cpp
    KWOdfWriter.cpp
    KWOdfSharedLoadingData.cpp
    KWRootAreaProvider.cpp

    pagetool/KWPageTool.cpp
    pagetool/KWPageToolFactory.cpp
    pagetool/SimpleSetupWidget.cpp
    pagetool/SimpleHeaderFooterWidget.cpp

    frames/KWFrameLayout.cpp
    frames/KWFrame.cpp
    frames/KWFrameSet.cpp
    frames/KWTextFrameSet.cpp
    frames/KWCopyShape.cpp
    frames/KWOutlineShape.cpp

    dialogs/KWFrameConnectSelector.cpp
    dialogs/KWRunAroundProperties.cpp
    dialogs/KWGeneralFrameProperties.cpp
    dialogs/KWAnchoringProperties.cpp
    dialogs/KWFrameDialog.cpp
    dialogs/KWShapeConfigFactory.cpp
    dialogs/KWDocumentColumns.cpp
    dialogs/KWStartupWidget.cpp
    dialogs/KWPageSettingsDialog.cpp
    dialogs/KWPrintingDialog.cpp
    dialogs/KWCreateBookmarkDialog.cpp
    dialogs/KWSelectBookmarkDialog.cpp
    dialogs/KWConfigureDialog.cpp

    dockers/KWStatisticsDocker.cpp
    dockers/KWStatisticsWidget.cpp
    dockers/StatisticsPreferencesPopup.cpp

    commands/KWPageStylePropertiesCommand.cpp
    commands/KWPageInsertCommand.cpp
    commands/KWPageRemoveCommand.cpp
    commands/KWNewPageStyleCommand.cpp
    commands/KWFrameDeleteCommand.cpp
    commands/KWFrameCreateCommand.cpp
    commands/KWChangePageStyleCommand.cpp
    commands/KWShapeCreateCommand.cpp

##calligra author specific additions
    author/CAuFactory.cpp
    author/CAuView.cpp
    author/CAuPart.cpp
    author/CoverImage.cpp
    author/CoverSelectionDialog.cpp
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
    dialogs/KWRunAroundProperties.ui
    dialogs/KWGeneralFrameProperties.ui
    dialogs/KWAnchoringProperties.ui
    dialogs/KWDocumentColumns.ui
    dialogs/KWStartupWidget.ui
    dialogs/KWCreateBookmark.ui
    dialogs/KWSelectBookmark.ui
    dialogs/KWInsertImage.ui
    dockers/StatisticsPreferencesPopup.ui
    pagetool/SimpleSetupWidget.ui
    pagetool/SimpleHeaderFooterWidget.ui
    author/CoverSelectionDialog.ui
)
if( Soprano_FOUND )
    kde4_add_ui_files(wordsprivate_LIB_SRCS
        dockers/KWRdfDocker.ui
    )
endif( Soprano_FOUND )


kde4_add_library(wordsprivate SHARED ${wordsprivate_LIB_SRCS})

target_link_libraries(wordsprivate komain textlayout)
if( Soprano_FOUND )
    target_link_libraries(wordsprivate kordf)
endif( Soprano_FOUND )
target_link_libraries(wordsprivate LINK_INTERFACE_LIBRARIES komain)

set_target_properties(wordsprivate
    PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS wordsprivate  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### WordsPart Library ###############

set(wordspart_PART_SRCS KWFactoryInit.cpp )


kde4_add_plugin(wordspart ${wordspart_PART_SRCS})



target_link_libraries(wordspart ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} wordsprivate )

install(TARGETS wordspart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES words.rc words_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/words)
install( FILES wordspart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES wordsrc DESTINATION ${CONFIG_INSTALL_DIR} )

install( FILES
words_export.h
KWPage.h
KWPageManager.h
KWPageStyle.h
Words.h
KWCanvasBase.h
KWCanvas.h
KWCanvasItem.h
KWDocument.h
KWApplicationConfig.h
KWViewMode.h
KWView.h
KWViewModeNormal.h
DESTINATION ${INCLUDE_INSTALL_DIR}/words/part COMPONENT Devel)

install( FILES
frames/KWFrame.h
frames/KWFrameLayout.h
frames/KWFrameSet.h
frames/KWTextFrameSet.h
DESTINATION ${INCLUDE_INSTALL_DIR}/words/part/frames COMPONENT Devel)


###What follows is calligraauthor section###

if (SHOULD_BUILD_AUTHOR)

########### AuthorPart Library ###############

set(authorpart_PART_SRCS author/CAuFactoryInit.cpp )

kde4_add_plugin(authorpart ${authorpart_PART_SRCS})

target_link_libraries(authorpart ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} wordsprivate )

install(TARGETS authorpart DESTINATION ${PLUGIN_INSTALL_DIR})

########### Author exe ###############

set(author_KDEINIT_SRCS author/main.cpp )

#kde4_add_app_icon(author_KDEINIT_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/author/hi*-app-calligraauthor.png")

kde4_add_kdeinit_executable( calligraauthor ${author_KDEINIT_SRCS})

if (Q_WS_MAC)
   set_target_properties(calligraauthor PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../app/Info.plist.template)
   set_target_properties(calligraauthor PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.calligra.author")
   set_target_properties(calligraauthor PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Author")
endif (Q_WS_MAC)

target_link_libraries(kdeinit_calligraauthor  ${KDE4_KDEUI_LIBS} komain )

install(TARGETS kdeinit_calligraauthor  ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( calligraauthor kdeinit_calligraauthor )
install(TARGETS calligraauthor  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES author/author.rc author/author_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/author)
install( FILES author/authorpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( PROGRAMS author/author.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES author/authorrc DESTINATION ${CONFIG_INSTALL_DIR} )

endif (SHOULD_BUILD_AUTHOR)
