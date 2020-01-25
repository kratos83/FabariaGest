# - try to find Qwt libraries and include files
# QWTQT5_INCLUDE_DIR where to find qwt_plot.h, etc.
# QWTQT5_LIBRARIES libraries to link against
# QWTQT5_FOUND If false, do not try to use Qwt

IF(FABARIAGEST_BUILD_WIN32)
find_path (QWT_INCLUDE_DIRS
	NAMES qwt_plot.h
	PATHS
	${MINGW_PREFIX}/include/qt5/qwt
	${MINGW_PREFIX}/include/qt5/qwt6
)
endIF(FABARIAGEST_BUILD_WIN32)

IF(FABARIAGEST_BUILD_WIN64)
find_path (QWT_INCLUDE_DIRS
	NAMES qwt_plot.h
	PATHS
	${MINGW_PREFIX}/include/qt5/qwt
	${MINGW_PREFIX}/include/qt5/qwt6
)
endIF(FABARIAGEST_BUILD_WIN64)


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

find_path (QWTQT5_INCLUDE_DIR qwt_plot.h
	
	/usr/local/include/qt5/qwt-qt5
	/usr/local/include/qt5/qwt
	/usr/local/include/qt5/qwt6
	/usr/include/qt5/qwt-qt5
	/usr/include/qt5/qwt
	/usr/include/qt5/qwt6
	/opt/include/qt5/qwt-qt5
	/opt/include/qt5/qwt
	/opt/include/qt5/qwt6
)

find_library (QWTQT5_LIBRARIES
	NAMES qwt-qt5
	PATHS /usr/local/lib /usr/lib
	      /usr/lib64
)

IF (QWTQT5_INCLUDE_DIR AND QWTQT5_LIBRARIES)
   SET(QWTQT5_FOUND TRUE)
ENDIF (QWTQT5_INCLUDE_DIR AND QWTQT5_LIBRARIES)

# handle the QUIETLY and REQUIRED arguments and set QWTQT5_FOUND to TRUE if 
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Qwt-qt5 DEFAULT_MSG QWTQT5_LIBRARIES QWTQT5_INCLUDE_DIR )
MARK_AS_ADVANCED(QWTQT5_LIBRARIES QWTQT5_INCLUDE_DIR)
