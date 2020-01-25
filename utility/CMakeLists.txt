project(backup_restore)

cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

if(WIN32)
SET( CMAKE_CXX_FLAGS  "-std=c++11 -U__STRICT_ANSI__ -mwindows" )
ENDIF()

set(backup_restore "backup_restore")

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Qt5Svg REQUIRED)


set(backup_restore_includes
${CMAKE_BINARY_DIR}
)

include_directories(${backup_restore_includes})

set(backup_restore_uis 
	db_bk_rs.ui	
)


qt5_wrap_ui(backup_restore_ui_hdrs ${backup_restore_uis})


set(backup_restore_moc_hdrs 
	db_bk_rs.h
)

qt5_wrap_cpp(backup_restore_mocs ${backup_restore_moc_hdrs})


set(backup_restore_srcs 
	db_bk_rs.cpp
	main.cpp
)

qt5_add_resources(RESOURCE_FILES images.qrc) 


IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES})
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)

IF(WIN32)
	SET(WINRC "${CMAKE_CURRENT_BINARY_DIR}/backup_restore.obj")
	ADD_DEFINITIONS(-D__USE_MINGW_ANSI_STDIO=1)
	ADD_CUSTOM_COMMAND(OUTPUT ${WINRC}
						COMMAND ${WINDRES}
							-I${CMAKE_CURRENT_SOURCE_DIR}
							-o${WINRC}
							-i${CMAKE_CURRENT_BINARY_DIR}/winicon.rc
						DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/winicon.rc)
ENDIF(WIN32)

LINK_LIBRARIES(-lpthread)
IF(APPLE)
set(SOURCE_FILES ${SOURCE_FILES} images/backup_restor.icns)
set_source_files_properties(images/backup_restor.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
ADD_EXECUTABLE( backup_restore  MACOSX_BUNDLE ${backup_restore_srcs} ${backup_restore_ui_hdrs} ${backup_restore_mocs} ${RESOURCE_FILES} ${SOURCE_FILES} ${WINRC})
ELSE()
ADD_EXECUTABLE( backup_restore  ${backup_restore_srcs} ${backup_restore_ui_hdrs} ${backup_restore_mocs} ${RESOURCE_FILES} ${SOURCE_FILES} ${WINRC})
ENDIF()

target_link_libraries( backup_restore
  Qt5::Widgets Qt5::Xml
  Qt5::Network Qt5::Gui 
  Qt5::Sql Qt5::Core 
  Qt5::Svg
)


IF(WIN32)
    INSTALL( TARGETS backup_restore RUNTIME DESTINATION .)
ELSE()
    INSTALL( TARGETS backup_restore RUNTIME DESTINATION /opt/fabaria_gest)
endif()

# set up apple vars before traversing into data/scripts
SET(MACOSX_BUNDLE_ICON_FILE "backup_restor.icns")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "Backup and restore")
SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_NAME "Backup and restore")
SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}")
SET(MACOSX_BUNDLE_COPYRIGHT "Codelinsoft, 2011-2015")
SET(MACOSX_BUNDLE_MIMETYPE "application/x-fabariagest-project")
SET(MACOSX_BUNDLE_MIMETYPE_ICON "backup_restor.icns")
SET(MACOSX_BUNDLE_MIMETYPE_ID "http://www.codelinsoft.it")
SET(MACOSX_BUNDLE_PROJECT_URL "http://www.codelinsoft.it")
