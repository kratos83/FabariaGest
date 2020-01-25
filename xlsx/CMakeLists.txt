project(xlsx)

cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

if(WIN32)
SET( CMAKE_CXX_FLAGS  "-DUNICODE -DQT_NO_MTDEV -DQT_NO_LIBUDEV -DQT_NO_EVDEV -DQT_NO_TSLIB -DQT_NO_LIBINPUT -DQT_BUILD_XLSX_LIB -DQT_BUILDING_QT -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES -DQT_NO_CAST_TO_ASCII -DQT_ASCII_CAST_WARNINGS -DQT_MOC_COMPAT -DQT_USE_QSTRINGBUILDER -DQT_DEPRECATED_WARNINGS -DQT_DISABLE_DEPRECATED_BEFORE=0x040800 -DQT_NO_EXCEPTIONS -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_OPENGL_ES_2 -DQT_OPENGL_ES_2_ANGLE -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 --param=ssp-buffer-size=4 -fno-keep-inline-dllexport -msse2 -mstackrealign -mfpmath=sse -std=c++0x -fno-exceptions -frtti -Wall -Wextra" )
ELSE()
SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11" )
ENDIF()

IF(WIN32)
set(Qt5Gui_PRIVATE_INCLUDE_DIRS
        "${MINGW_PREFIX}/include/qt/QtGui/5.12.5"
        "${MINGW_PREFIX}/include/qt/QtGui/5.12.5/QtGui"
    )
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} 
		    ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
ELSE()
include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} 
		    ${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_PRIVATE_INCLUDE_DIRS})
ENDIF()
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)

set(xlsx "xlsx")

set(xlsx_includes
${CMAKE_BINARY_DIR}/xlsx
)

include_directories(${xlsx_includes})

set(xlsx_moc_hdrs 
	xlsxdocpropscore_p.h 
    xlsxdocpropsapp_p.h 
    xlsxrelationships_p.h 
    xlsxutility_p.h 
    xlsxsharedstrings_p.h 
    xlsxcontenttypes_p.h 
    xlsxtheme_p.h 
    xlsxformat.h 
    xlsxworkbook.h 
    xlsxstyles_p.h 
    xlsxabstractsheet.h 
    xlsxabstractsheet_p.h 
    xlsxworksheet.h 
    xlsxworksheet_p.h 
    xlsxchartsheet.h 
    xlsxchartsheet_p.h 
    xlsxzipwriter_p.h 
    xlsxworkbook_p.h 
    xlsxformat_p.h 
    xlsxglobal.h 
    xlsxdrawing_p.h 
    xlsxzipreader_p.h 
    xlsxdocument.h 
    xlsxdocument_p.h 
    xlsxcell.h 
    xlsxcell_p.h 
    xlsxdatavalidation.h 
    xlsxdatavalidation_p.h 
    xlsxcellreference.h 
    xlsxcellrange.h 
    xlsxrichstring_p.h 
    xlsxrichstring.h 
    xlsxconditionalformatting.h 
    xlsxconditionalformatting_p.h 
    xlsxcolor_p.h 
    xlsxnumformatparser_p.h 
    xlsxdrawinganchor_p.h 
    xlsxmediafile_p.h 
    xlsxabstractooxmlfile.h 
    xlsxabstractooxmlfile_p.h 
    xlsxchart.h 
    xlsxchart_p.h 
    xlsxsimpleooxmlfile_p.h 
    xlsxcellformula.h 
    xlsxcellformula_p.h
)

qt5_wrap_cpp(xlsx_mocs ${xlsx_moc_hdrs})


set(xlsx_srcs 
	xlsxdocpropscore.cpp 
    xlsxdocpropsapp.cpp 
    xlsxrelationships.cpp 
    xlsxutility.cpp 
    xlsxsharedstrings.cpp 
    xlsxcontenttypes.cpp 
    xlsxtheme.cpp 
    xlsxformat.cpp 
    xlsxstyles.cpp 
    xlsxworkbook.cpp 
    xlsxabstractsheet.cpp 
    xlsxworksheet.cpp 
    xlsxchartsheet.cpp 
    xlsxzipwriter.cpp 
    xlsxdrawing.cpp 
    xlsxzipreader.cpp 
    xlsxdocument.cpp 
    xlsxcell.cpp 
    xlsxdatavalidation.cpp 
    xlsxcellreference.cpp 
    xlsxcellrange.cpp 
    xlsxrichstring.cpp 
    xlsxconditionalformatting.cpp 
    xlsxcolor.cpp 
    xlsxnumformatparser.cpp 
    xlsxdrawinganchor.cpp 
    xlsxmediafile.cpp 
    xlsxabstractooxmlfile.cpp 
    xlsxchart.cpp 
    xlsxsimpleooxmlfile.cpp 
    xlsxcellformula.cpp
)

LINK_LIBRARIES(-lpthread)

IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES})
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)

LINK_LIBRARIES(-lpthread Qt5::Widgets Qt5::Gui Qt5::Core  )

ADD_LIBRARY(xlsx_fabaria SHARED ${xlsx_srcs} ${xlsx_mocs})


IF(WIN32)
    INSTALL( TARGETS xlsx_fabaria RUNTIME DESTINATION .)
else()
    INSTALL( TARGETS xlsx_fabaria LIBRARY DESTINATION ${LIB_DIR})
endif()
