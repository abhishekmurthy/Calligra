project(pigmentcms)

include_directories( ${PIGMENT_INCLUDES} ${QT_INCLUDES} ${Boost_INCLUDE_DIR})

set(FILE_OPENEXR_SOURCES)
set(LINK_OPENEXR_LIB)
if(OPENEXR_FOUND)
    include_directories(${OPENEXR_INCLUDE_DIR})
    set(LINK_OPENEXR_LIB ${OPENEXR_LIBRARIES})
    add_definitions(${OPENEXR_DEFINITIONS})
endif(OPENEXR_FOUND)

set(LINK_VC_LIB)

if(HAVE_VC)
    include_directories(${Vc_INCLUDE_DIR})
    set(LINK_VC_LIB ${Vc_LIBRARIES})
    ko_compile_for_all_implementations_no_scalar(__per_arch_factory_objs compositeops/KoOptimizedCompositeOpFactoryPerArch.cpp)

    message("Following objects are generated from the per-arch lib")
    message(${__per_arch_factory_objs})
endif(HAVE_VC)

add_subdirectory(tests)
add_subdirectory(benchmarks)

set(pigmentcms_SRCS
    KoBasicHistogramProducers.cpp
    KoColor.cpp
    KoColorConversionAlphaTransformation.cpp
    KoColorConversionCache.cpp
    KoColorConversions.cpp
    KoColorConversionSystem.cpp
    KoColorConversionTransformation.cpp
    KoColorConversionTransformationFactory.cpp
    KoColorModelStandardIds.cpp
    KoColorProfile.cpp
    KoColorSpace.cpp
    KoColorSpaceEngine.cpp
    KoColorSpaceFactory.cpp
    KoColorSpaceMaths.cpp
    KoColorSpaceRegistry.cpp
    KoColorTransformation.cpp
    KoColorTransformationFactory.cpp
    KoColorTransformationFactoryRegistry.cpp
    KoCompositeOp.cpp
    KoCompositeOpRegistry.cpp
    KoCopyColorConversionTransformation.cpp
    KoFallBackColorTransformation.cpp
    KoHistogramProducer.cpp
    KoMultipleColorConversionTransformation.cpp
    KoUniqueNumberForIdServer.cpp
    colorspaces/KoAlphaColorSpace.cpp
    colorspaces/KoLabColorSpace.cpp
    colorspaces/KoRgbU16ColorSpace.cpp
    colorspaces/KoRgbU8ColorSpace.cpp
    colorspaces/KoSimpleColorSpaceEngine.cpp
    compositeops/KoOptimizedCompositeOpFactory.cpp
    compositeops/KoOptimizedCompositeOpFactoryPerArch_Scalar.cpp
    ${__per_arch_factory_objs}
    colorprofiles/KoDummyColorProfile.cpp
    resources/KoAbstractGradient.cpp
    resources/KoColorSet.cpp
    resources/KoPattern.cpp
    resources/KoResource.cpp
    resources/KoStopGradient.cpp
    resources/KoSegmentGradient.cpp
)

set(PIGMENT_INSTALL_FILES 
        pigment_export.h
        DebugPigment.h
        KoBasicHistogramProducers.h
        KoChannelInfo.h
        KoColor.h
        KoColorConversionTransformation.h
        KoColorConversionTransformationAbstractFactory.h
        KoColorConversionTransformationFactory.h
        KoColorModelStandardIds.h
        KoColorProfile.h
        KoColorSpace.h
        KoColorSpaceEngine.h
        KoColorSpaceFactory.h
        KoColorSpaceAbstract.h
        KoColorSpaceConstants.h
        KoColorSpaceMaths.h
        KoColorSpaceRegistry.h
        KoCmykColorSpaceTraits.h
        KoColorSpaceTraits.h
        KoGrayColorSpaceTraits.h
        KoLabColorSpaceTraits.h
        KoRgbColorSpaceTraits.h
        KoXyzColorSpaceTraits.h
        KoYcbcrColorSpaceTraits.h
        KoColorTransformation.h
        KoColorTransformationFactory.h
        KoColorTransformationFactoryRegistry.h
        KoCompositeOp.h
        KoConvolutionOp.h
        KoFallBackColorTransformation.h
        KoIntegerMaths.h
        KoLabDarkenColorTransformation.h
        KoMixColorsOp.h
        KoMixColorsOpImpl.h
        KoHistogramProducer.h
)

set (EXTRA_LIBRARIES ${LINK_OPENEXR_LIB} ${LINK_VC_LIB})

if(MSVC)
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif(MSVC)

kde4_add_library(pigmentcms SHARED ${pigmentcms_SRCS})

target_link_libraries(
    pigmentcms
    koplugin
    ${EXTRA_LIBRARIES}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    )

target_link_libraries(
    pigmentcms LINK_INTERFACE_LIBRARIES koplugin ${QT_QTXML_LIBRARY} ${QT_QTGUI_LIBRARY} ${EXTRA_LIBRARIES}
)

set_target_properties(pigmentcms PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)

install(TARGETS pigmentcms  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES ${PIGMENT_INSTALL_FILES}
         DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

install( FILES pigment.desktop pigmentextension.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
