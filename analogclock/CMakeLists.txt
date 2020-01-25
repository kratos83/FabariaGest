cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)

set(anaclogclock "anaclogclock")

set(analogclock_includes
${CMAKE_BINARY_DIR}/analogclock
)

include_directories(${analogclock_includes})

set(anaclogclock_moc_hdrs 
	analogclock.h
)

qt5_wrap_cpp(anaclogclock_mocs ${anaclogclock_moc_hdrs})

set(anaclogclock_srcs 
	analogclock.cpp
)


qt5_add_resources(RESOURCE_FILES images.qrc) 

IF(WIN32)
    LINK_LIBRARIES(-lws2_32 -lwtsapi32 -lnetapi32 -luserenv -ladvapi32 ${QT_LIBRARIES})
    SET(CMAKE_MODULE_LINKER_FLAGS -Wl,-export-all-symbols)
endif(WIN32)

IF(WIN64)
ADD_DEFINITIONS(-D_X64)
ENDIF(WIN64)

LINK_LIBRARIES(-lpthread Qt5::Widgets Qt5::Gui Qt5::Core  )

ADD_LIBRARY(anaclogclock_fabaria SHARED ${anaclogclock_srcs} ${anaclogclock_mocs} ${RESOURCE_FILES})


IF(WIN32)
    INSTALL( TARGETS anaclogclock_fabaria RUNTIME DESTINATION .)
else()
    INSTALL( TARGETS anaclogclock_fabaria LIBRARY DESTINATION ${LIB_DIR})
endif()
