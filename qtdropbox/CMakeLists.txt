SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11" )

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Xml REQUIRED)
FIND_PACKAGE(Qt5Network REQUIRED)
FIND_PACKAGE(Qt5Sql REQUIRED)
FIND_PACKAGE(Qt5Svg REQUIRED)
              
FILE(GLOB ic_INCLUDES
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/async/*.h
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/auth/*.h
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/endpoint/*.h
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/files/*.h 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/properties/*.h 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/sharing/*.h 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/team/*.h
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/team_common/*.h 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/team_policies/*.h 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/users/*.h
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/*.h)
FILE(GLOB ic_SOURCES
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/async/*.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/auth/*.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/endpoint/*.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/files/*.cpp 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/properties/*.cpp 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/sharing/*.cpp 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/team/*.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/team_common/*.cpp 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/team_policies/*.cpp 
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/users/*.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/dropbox/*.cpp)


QT5_WRAP_CPP(ic_MOC_out ${ic_INCLUDES})


IF(WIN32)
	LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 )
	SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
LINK_LIBRARIES(-lpthread Qt5::Widgets Qt5::Xml
  Qt5::Network Qt5::Gui 
  Qt5::Sql Qt5::Core 
  Qt5::Svg )
ADD_LIBRARY(qtdropbox_fabaria SHARED ${ic_SOURCES} ${ic_INCLUDES} ${ic_MOC_out} ${ic_RCC_out} )

IF(WIN32)
	INSTALL(TARGETS qtdropbox_fabaria RUNTIME DESTINATION .)
ELSE()
	INSTALL( TARGETS qtdropbox_fabaria LIBRARY DESTINATION ${LIB_DIR})
ENDIF(WIN32) 

