project(quazip)

cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )
SET( CMAKE_CXX_FLAGS  "-std=c++11 -U__STRICT_ANSI__ -fPIC" )

set(quazip "quazip")

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} 
		    ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS}
		    ${Qt5PrintSupport_INCLUDE_DIRS})
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)

ADD_DEFINITIONS(-DQUAZIP_BUILD)

set(quazip_includes
${CMAKE_BINARY_DIR}
)

include_directories(${quazip_includes})


set(quazip_moc_hdrs 
	crypt.h 
        ioapi.h 
        JlCompress.h 
        quaadler32.h 
        quachecksum32.h 
        quacrc32.h 
        quagzipfile.h 
        quaziodevice.h 
        quazipdir.h 
        quazipfile.h 
        quazipfileinfo.h 
        quazip_global.h 
        quazip.h 
        quazipnewinfo.h 
        unzip.h 
        zip.h
)

qt5_wrap_cpp(quazip_mocs ${quazip_moc_hdrs})


set(quazip_srcs 
	   qioapi.cpp 
           JlCompress.cpp 
           quaadler32.cpp 
           quacrc32.cpp 
           quagzipfile.cpp 
           quaziodevice.cpp 
           quazip.cpp 
           quazipdir.cpp 
           quazipfile.cpp 
           quazipfileinfo.cpp 
           quazipnewinfo.cpp 
           unzip.c 
           zip.c
)

IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES} )
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)

LINK_LIBRARIES(-lpthread )

IF(WIN32)
ADD_LIBRARY( quazip_fabaria SHARED ${quazip_srcs} ${quazip_ui_hdrs} ${quazip_mocs} ${RESOURCE_FILES})
target_link_libraries(quazip_fabaria Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Sql Qt5::PrintSupport ${ZLIB_LIBRARIES})
ELSE()
ADD_LIBRARY( quazip_fabaria SHARED ${quazip_srcs} ${quazip_ui_hdrs} ${quazip_mocs} ${RESOURCE_FILES} ${ZLIB_LIBRARIES})
target_link_libraries(quazip_fabaria Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Sql Qt5::PrintSupport ${ZLIB_LIBRARIES})
ENDIF(WIN32)

IF(WIN32)
    INSTALL( TARGETS quazip_fabaria RUNTIME DESTINATION .)
ELSE()
    INSTALL( TARGETS quazip_fabaria LIBRARY DESTINATION ${LIB_DIR})
endif()
