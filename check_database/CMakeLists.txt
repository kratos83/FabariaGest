project(check_database)
cmake_policy(SET CMP0020 NEW)
cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -U__STRICT_ANSI__ -fPIC" )

set(check_database "check_database")

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} 
		    ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS})
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)


set(check_database_includes
${CMAKE_BINARY_DIR}
)

include_directories(${check_database_includes})

set(check_database_uis 
	check_database.ui	
)

qt5_wrap_ui(check_database_ui_hdrs ${check_database_uis})


set(check_database_moc_hdrs 
	check_database.h
	checkdatabase_plugin.h
	../fabaria/databasesql.h
	../fabaria/defines.h
	../fabaria/queryexception.h
)

qt5_wrap_cpp(check_database_mocs ${check_database_moc_hdrs})


set(check_database_srcs 
	check_database.cpp
	checkdatabase_plugin.cpp
	../fabaria/databasesql.cpp
	../fabaria/defines.cpp
)

IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES})
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)

LINK_LIBRARIES(-lpthread Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Sql Qt5::PrintSupport  )


ADD_LIBRARY( check_database SHARED ${check_database_srcs} ${check_database_ui_hdrs} ${check_database_mocs} ${RESOURCE_FILES})

IF(WIN32)
    INSTALL( TARGETS check_database RUNTIME DESTINATION ./plugin)
ELSE()
    INSTALL( TARGETS check_database LIBRARY DESTINATION /opt/FabariaGest/plugin)
endif()
