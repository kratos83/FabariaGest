project(backup_restore_db)
cmake_minimum_required(VERSION 3.1)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )
SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -U__STRICT_ANSI__ -fPIC" )

set(backup_restore_db "backup_restore_db")

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Core_INCLUDE_DIRS} ${Qt5Sql_INCLUDE_DIRS} )
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)


set(backup_restore_db_includes
${CMAKE_BINARY_DIR}
)

include_directories(${backup_restore_db_includes})



set(backup_restore_db_uis 
	db_bk_rs.ui	
)

qt5_wrap_ui(backup_restore_db_ui_hdrs ${backup_restore_db_uis})

set(backup_restore_db_moc_hdrs 
	db_bk_rs.h
	dbbkrs_plugin.h
	../fabaria/databasesql.h
	../fabaria/defines.h
	../fabaria/queryexception.h
)

qt5_wrap_cpp(backup_restore_db_mocs ${backup_restore_db_moc_hdrs})


set(backup_restore_db_srcs 
	db_bk_rs.cpp
	dbbkrs_plugin.cpp
	../fabaria/databasesql.cpp
	../fabaria/defines.cpp
)

qt5_add_resources(RESOURCE_FILES images.qrc) 


IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES})
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)

LINK_LIBRARIES(-lpthread Qt5::Widgets Qt5::Gui Qt5::Core Qt5::Sql  )


ADD_LIBRARY( backup_restore_db SHARED ${backup_restore_db_srcs} ${backup_restore_db_ui_hdrs} ${backup_restore_db_mocs} ${RESOURCE_FILES})

IF(WIN32)
    INSTALL( TARGETS backup_restore_db RUNTIME DESTINATION ./plugin)
ELSE()
    INSTALL( TARGETS backup_restore_db LIBRARY DESTINATION /opt/FabariaGest/plugin)
endif()
