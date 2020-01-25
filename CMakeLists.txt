CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

PROJECT(fabariagest)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0005 NEW)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

INCLUDE(AddFileDependencies)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)

SET(VERSION_MAJOR "2")
SET(VERSION_MINOR "1")

#SET(VERSION_SUFFIX "")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}")
IF(VERSION_SUFFIX)
	SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)

INCLUDE(DetectMachine)


LIST(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}")


set(QT5 TRUE)
	
message(STATUS "Checking for Qt5...")
find_package(Qt5 REQUIRED Widgets Concurrent Qml Quick QuickWidgets 
              PrintSupport Gui Core Xml Network Sql Svg Multimedia MultimediaWidgets )

message(STATUS "Checking for ZLIB...")
find_package(ZLIB REQUIRED)

if(FABARIAGEST_BUILD_LINUX)
    message(STATUS "Checking for LSB...")
    find_package(LSB REQUIRED)
ENDIF(FABARIAGEST_BUILD_LINUX)
	
# set compiler flags
SET(WERROR_FLAGS "-Wall -Werror=unused-function -Wno-sign-compare -Wno-strict-overflow")
OPTION(USE_WERROR "Add -werror to the build flags. Stops the build on warnings" OFF)
IF(${USE_WERROR})
   SET(WERROR_FLAGS "${WERROR_FLAGS} -Werror")
ENDIF()

SET(CMAKE_C_FLAGS "${WERROR_FLAGS} ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "-fno-exceptions ${WERROR_FLAGS} ${CMAKE_CXX_FLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DFABARIAGEST_DEBUG")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DFABARIAGEST_DEBUG")

# make sub-directories
ADD_SUBDIRECTORY(analogclock)
ADD_SUBDIRECTORY(backup_restore_db)
ADD_SUBDIRECTORY(check_database)
ADD_SUBDIRECTORY(qtdropbox)
ADD_SUBDIRECTORY(smtp)
ADD_SUBDIRECTORY(inventario)
ADD_SUBDIRECTORY(xlsx)
ADD_SUBDIRECTORY(log_man)
ADD_SUBDIRECTORY(qwt)
ADD_SUBDIRECTORY(quazip)
ADD_SUBDIRECTORY(initdb)
ADD_SUBDIRECTORY(fabaria)
ADD_SUBDIRECTORY(update)
ADD_SUBDIRECTORY(utility)

#
# add distclean-target
#
ADD_CUSTOM_TARGET(distclean
			COMMAND make clean
			COMMAND rm -rf `find -name cmake_install.cmake` `find -name Makefile` `find -type d -name CMakeFiles` CMakeCache.txt)

#
# display configuration information
#

MESSAGE("\n"
"Installation Summary\n"
"--------------------\n"
"* Install Directory           : ${CMAKE_INSTALL_PREFIX}\n"
"* Build type                  : ${CMAKE_BUILD_TYPE}\n"
"* Platform                    : ${CMAKE_SYSTEM_PROCESSOR} (${Machine})\n"
)

MESSAGE(
"\n"
"-----------------------------------------------------------------\n"
"IMPORTANT:\n"
"after installing missing packages, remove CMakeCache.txt before\n"
"running cmake again!\n"
"-----------------------------------------------------------------\n"
"\n\n")

INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "FABARIAGEST - software aziendale!")
SET(CPACK_PACKAGE_VENDOR "Codelinfoft Developers")

SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")

IF(VERSION_SUFFIX)
	SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "FabariaGest")

	SET(CPACK_GENERATOR NSIS)
	SET(CPACK_NSIS_DEFINES "!include FileAssociation.nsh")
	SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "
		!define MUI_ICON  \\\"${CMAKE_SOURCE_DIR}/fabaria/images/logo4.ico\\\"
		!define MUI_HEADERIMAGE_BITMAP \\\"${CMAKE_SOURCE_DIR}/fabaria/images/wizard_s.bmp\\\"
  		!define MUI_WELCOMEFINISHPAGE_BITMAP \\\"${CMAKE_SOURCE_DIR}/fabaria/images/wizard.bmp\\\" 
		!define MUI_UNIICON \\\"${CMAKE_SOURCE_DIR}/fabaria/images/logo4.ico\\\" ")
	SET(CPACK_NSIS_INSTALLER_MUI_FINISHPAGE_RUN_CODE " !define MUI_FINISHPAGE_RUN \\\"$INSTDIR\\\\fabaria_gest.exe\\\" 							          
                !define MUI_FINISHPAGE_SHOWREADME \\\"$INSTDIR\\\\README.txt\\\" ")
	SET(CPACK_NSIS_INSTALLED_ICON_NAME "fabaria_gest.exe")
	SET(CPACK_NSIS_DISPLAY_NAME "FabariaGest ${VERSION}")
	SET(CPACK_NSIS_HELP_LINK "http://www.codelinsoft.it")
	SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.codelinsoft.it")
	SET(CPACK_NSIS_CONTACT "info@codelinsoft.it")
	SET(CPACK_PACKAGE_EXECUTABLES "fabaria_gest.exe;FabariaGest")
	SET(CPACK_NSIS_MENU_LINKS "fabaria_gest.exe;FabariaGest")
	SET(CPACK_CREATE_DESKTOP_LINKS "fabaria_gest.exe;FabariaGest")
	set(CPACK_NSIS_CREATE_ICONS_EXTRA "
    	CreateShortCut \\\"$DESKTOP\\\\FabariaGest.lnk\\\" \\\"$INSTDIR\\\\fabaria_gest.exe\\\" 
	CreateShortCut	\\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Backup-ripristino database.lnk\\\" \\\"$INSTDIR\\\\backup_restore.exe\\\" 
	CreateShortCut	\\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\FabariaGest url.lnk\\\" \\\"$INSTDIR\\\\sito.html\\\" ")
	set(CPACK_NSIS_DELETE_ICONS_EXTRA "
    		Delete \\\"$DESKTOP\\\\FabariaGest.lnk\\\"
		Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Backup-ripristino database.lnk\\\" 
		Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\FabariaGest url.lnk\\\" ")
	SET(CPACK_PACKAGE_FILE_NAME "FabariaGest-${VERSION}-win64")
	SET(CPACK_NSIS_DEFINES "${CPACK_NSIS_DEFINES}
		InstallDir \\\"\\\$PROGRAMFILES64\\\\${CPACK_PACKAGE_INSTALL_DIRECTORY}\\\"")

INCLUDE(CPack)