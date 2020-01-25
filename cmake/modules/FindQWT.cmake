# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_plot.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt

IF(FABARIAGEST_BUILD_WIN32)
find_path (QWT_INCLUDE_DIRS
	NAMES qwt_plot.h
	PATHS
	${MINGW_PREFIX}/include/qwt
	${MINGW_PREFIX}/include/qw6
)
endIF(FABARIAGEST_BUILD_WIN32)

IF(FABARIAGEST_BUILD_WIN64)
find_path (QWT_INCLUDE_DIRS
	NAMES qwt_plot.h
	PATHS
	${MINGW_PREFIX}/include/qwt
	${MINGW_PREFIX}/include/qwt6
)
endIF(FABARIAGEST_BUILD_WIN64)

if(FABARIAGEST_BUILD_LINUX)
find_path (QWT_INCLUDE_DIRS
	NAMES qwt_plot.h
	PATHS
	/usr/local/include/qwt-qt4
	/usr/local/include/qwt
	/usr/local/include/qwt6
	/usr/include/qwt-qt4
	/usr/include/qwt
	/usr/include/qwt6
)
endif(FABARIAGEST_BUILD_LINUX)

IF(FABARIAGEST_BUILD_WIN32)
find_library (QWT_LIBRARIES
	NAMES qwt-qt4 qwt
	PATHS ${MINGW_PREFIX}/lib
)
ENDIF(FABARIAGEST_BUILD_WIN32)

IF(FABARIAGEST_BUILD_WIN64)
find_library (QWT_LIBRARIES
	NAMES qwt-qt4 qwt
	PATHS ${MINGW_PREFIX}/lib
)
ENDIF(FABARIAGEST_BUILD_WIN64)

if(FABARIAGEST_BUILD_LINUX)
find_library (QWT_LIBRARIES
	NAMES qwt-qt4 qwt
	PATHS /usr/local/lib /usr/lib
	      /usr/lib64
)
endif(FABARIAGEST_BUILD_LINUX)

# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if 
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS )
MARK_AS_ADVANCED(QWT_LIBRARIES QWT_INCLUDE_DIRS)
