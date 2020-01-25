cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )
SET( CMAKE_CXX_FLAGS  "-fexceptions" )


include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} 
		    ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} 
		    ${Qt5Network_INCLUDE_DIRS})
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)


set(smtp "smtp")

set(smtp_includes
${CMAKE_BINARY_DIR}/smtp
)

include_directories(${smtp_includes})

set(smtp_moc_hdrs 
	emailaddress.h
	mimeattachment.h
	mimecontentformatter.h
	mimefile.h
	mimehtml.h
	mimeinlinefile.h
	mimemessage.h
	mimemultipart.h
	mimepart.h
	mimetext.h
	quotedprintable.h
	smtpclient.h
	smtpexports.h
)


qt5_wrap_cpp(smtp_mocs ${smtp_moc_hdrs})


set(smtp_srcs 
	emailaddress.cpp
	mimeattachment.cpp
	mimecontentformatter.cpp
	mimefile.cpp
	mimehtml.cpp
	mimeinlinefile.cpp
	mimemessage.cpp
	mimemultipart.cpp
	mimepart.cpp
	mimetext.cpp
	quotedprintable.cpp
	smtpclient.cpp
)

IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES})
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols -fexceptions)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)


LINK_LIBRARIES(-lpthread Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Sql Qt5::Network  )


ADD_LIBRARY(smtp_fabaria SHARED ${smtp_srcs} ${smtp_mocs})


IF(WIN32)
    INSTALL( TARGETS smtp_fabaria RUNTIME DESTINATION .)
else()
    INSTALL( TARGETS smtp_fabaria LIBRARY DESTINATION ${LIB_DIR})
endif()
