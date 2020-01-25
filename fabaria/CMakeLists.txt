project(FabariaGest)
cmake_policy(SET CMP0020 NEW)
cmake_minimum_required(VERSION 2.6)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules )

if(WIN32)
SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -U__STRICT_ANSI__ -mwindows -fPIC" )
ELSE()
SET( CMAKE_CXX_FLAGS  "-fexceptions -std=c++11 -U__STRICT_ANSI__ -fPIC" )
ENDIF()

set(FabariaGest "FabariaGest")

set(fabaria_includes
${CMAKE_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/../qtdropbox
${CMAKE_CURRENT_SOURCE_DIR}/../xlsx
)

include_directories(${fabaria_includes} )

set(FabariaGest_moc_hdrs 
	about.h
	articoli.h
	azienda.h
	banche.h
	barcode.h
	button.h
	carico_mag.h
	categoria.h
	causali.h
	chiudi_anno_contabile.h
	classe.h
	cod_fisc.h
	comuni.h
	completer.h
	databasesql.cpp
	Debug.h
	ddt.h
	ddt_righe.h
	ddt_righe_art.h
	details_plugin.h
	dialog_fattura.h
	dialog_fattura_acq.h
	distinta_riba.h
	minitutorial.h
	DropboxFabaria.h
	selezionamag.h
	ean13.h
	esci.h
	f_rest.h
	fatt_acq.h
	fatt_acq_art.h
	fatt_new.h
	fattura.h
	fattura_elettronica.h
	fattura_rg.h
	fattura_rg_art.h
	fornitori.h
	frame.h
	image_resize.h
	imagescene.h
	impo_iva_toto.h
	imposta_anno.h
	itdelegato.h
	iva.h
	LineEditDelegate.h
	listino.h
	listaemail.h
	mainwindow.h
	magazzino.h
	misure.h
	ordine.h
	ordine_righe.h
	ordine_righe_art.h
	LineEdit.h
	pag.h
	pdf_export.h
	pluginmanager.h
	pref.h
	preventivi.h
	preventivi_rg.h
	preventivi_wid.h
	print.h
	progressbar.h
	resizedialog.h
	scadenze.h
	scarico_mag.h
	settingsmanager.h
	splashscreen.h
	user.h
	utils.h
	email_send.h
	ListBox.h
	lista_invio_mail.h
	esistenza_all.h
	esistenza_articoli.h
	tab_style.h
	agenti.h 
	riep_agent.h 
	reg_iva.h
	IconFileProvider.h
	defines.h
	inputdialogcombo.h
	fabariaapplication.h
)


qt5_wrap_cpp(FabariaGest_mocs ${FabariaGest_moc_hdrs})


set(FabariaGest_uis 
	ui/about.ui
	ui/articoli.ui
	ui/azienda.ui
	ui/banche.ui
	ui/barcode.ui
	ui/carico_mag.ui
	ui/categoria.ui
	ui/causali.ui
	ui/chiudi_anno_contabile.ui
	ui/cod_fisc.ui
	ui/comuni.ui
	ui/ddt.ui
	ui/ddt_righe.ui
	ui/ddt_righe_art.ui
	ui/details_plugin.ui
	ui/dialog_fattura.ui
	ui/minitutorial.ui
	ui/dialog_fattura_acq.ui
	ui/distinta_riba.ui
	ui/DropboxFabaria.ui
	ui/esci.ui
	ui/f_rest.ui
	ui/fatt_acq.ui
	ui/selezionamag.ui
	ui/fatt_acq_art.ui
	ui/fatt_new.ui
	ui/fattura.ui
	ui/fattura_elettronica.ui
	ui/fattura_rg.ui
	ui/fattura_rg_art.ui
	ui/fornitori.ui
	ui/imposta_anno.ui
	ui/iva.ui
	ui/listino.ui
	ui/listaemail.ui
	ui/mainwindow.ui
	ui/magazzino.ui
	ui/misure.ui
	ui/ordine.ui
	ui/ordine_righe.ui
	ui/ordine_righe_art.ui
	ui/pag.ui
	ui/pluginmanager.ui
	ui/pref.ui
	ui/preventivi.ui
	ui/preventivi_rg.ui
	ui/preventivi_wid.ui
	ui/scadenze.ui
	ui/scarico_mag.ui
	ui/user.ui
	ui/email_send.ui
	ui/lista_invio_mail.ui
	ui/esistenza_all.ui
	ui/esistenza_articoli.ui
	ui/agenti.ui
	ui/riep_agent.ui 
	ui/reg_iva.ui
	ui/inputdialogcombo.ui
)

qt5_wrap_ui(FabariaGest_ui_hdrs ${FabariaGest_uis})


set(FabariaGest_srcs 
	about.cpp
	articoli.cpp
	azienda.cpp
	banche.cpp
	barcode.cpp
	button.cpp
	carico_mag.cpp
	categoria.cpp
	causali.cpp
	chiudi_anno_contabile.cpp
	classe.cpp
	cod_fisc.cpp
	comuni.cpp
	completer.cpp
	databasesql.cpp
	Debug.cpp
	ddt.cpp
	ddt_righe.cpp
	ddt_righe_art.cpp
	details_plugin.cpp
	dialog_fattura.cpp
	dialog_fattura_acq.cpp
	distinta_riba.cpp
	DropboxFabaria.cpp
	ean13.cpp
	esci.cpp
	minitutorial.cpp
	LineEdit.cpp
	selezionamag.cpp
	f_rest.cpp
	fatt_acq.cpp
	fatt_acq_art.cpp
	fatt_new.cpp
	fattura.cpp
	fattura_elettronica.cpp
	fattura_rg.cpp
	fattura_rg_art.cpp
	fornitori.cpp
	frame.cpp
	image_resize.cpp
	imagescene.cpp
	impo_iva_toto.cpp
	imposta_anno.cpp
	itdelegato.cpp
	iva.cpp
	LineEditDelegate.cpp
	listino.cpp
	ListBox.cpp
	listaemail.cpp
	mainwindow.cpp
	magazzino.cpp
	main.cpp
	misure.cpp
	ordine.cpp
	ordine_righe.cpp
	ordine_righe_art.cpp
	pag.cpp
	pdf_export.cpp
	pluginmanager.cpp
	pref.cpp
	preventivi.cpp
	preventivi_rg.cpp
	preventivi_wid.cpp
	print.cpp
	progressbar.cpp
	resizedialog.cpp
	scadenze.cpp
	scarico_mag.cpp
	settingsmanager.cpp
	splashscreen.cpp
	user.cpp
	email_send.cpp
	lista_invio_mail.cpp
	tab_style.cpp
	esistenza_all.cpp
	esistenza_articoli.cpp
	agenti.cpp
	riep_agent.cpp 
	reg_iva.cpp
	defines.cpp
	IconFileProvider.cpp
	inputdialogcombo.cpp
	fabariaapplication.cpp
)

qt5_add_resources(RESOURCE_FILES fabaria.qrc) 


IF(WIN32)
	SET(WINRC "${CMAKE_CURRENT_BINARY_DIR}/fabariagest.obj")
	ADD_DEFINITIONS(-D__USE_MINGW_ANSI_STDIO=1)
	ADD_CUSTOM_COMMAND(OUTPUT ${WINRC}
						COMMAND ${WINDRES}
							-I${CMAKE_CURRENT_SOURCE_DIR}
							-o${WINRC}
							-i${CMAKE_CURRENT_BINARY_DIR}/winicon.rc
						DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/winicon.rc)

ENDIF(WIN32)

if (APPLE)
    set(SOURCE_FILES ${SOURCE_FILES} images/logo4.icns)
    set_source_files_properties(images/logo4.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    ADD_EXECUTABLE(FabariaGest MACOSX_BUNDLE ${FabariaGest_srcs} ${FabariaGest_mocs} ${FabariaGest_ui_hdrs} ${RESOURCE_FILES} ${WINRC} ${SOURCE_FILES})
else()
ADD_EXECUTABLE(FabariaGest ${FabariaGest_srcs} ${FabariaGest_mocs} ${FabariaGest_ui_hdrs} ${RESOURCE_FILES} ${WINRC} ${SOURCE_FILES})
endif()

target_link_libraries( FabariaGest
  anaclogclock_fabaria 
  initdb_fabaria
  log_man_fabaria
  smtp_fabaria
  xlsx_fabaria
  qwt_fabaria
  quazip_fabaria
  qtdropbox_fabaria
  Qt5::Widgets Qt5::Xml
  Qt5::Network Qt5::Gui 
  Qt5::Sql Qt5::Core 
  Qt5::Svg Qt5::PrintSupport 
  Qt5::Quick Qt5::QuickWidgets
  Qt5::Qml Qt5::Multimedia
  Qt5::MultimediaWidgets
)


IF(WIN32)
    INSTALL(TARGETS FabariaGest RUNTIME DESTINATION .)
	INSTALL(FILES
		"${MINGW_PREFIX}/bin/Qt5Core.dll"
		"${MINGW_PREFIX}/bin/Qt5Gui.dll"
		"${MINGW_PREFIX}/bin/Qt5Svg.dll"
		"${MINGW_PREFIX}/bin/Qt5Xml.dll"
		"${MINGW_PREFIX}/bin/Qt5Network.dll"
		"${MINGW_PREFIX}/bin/Qt5Multimedia.dll"
		"${MINGW_PREFIX}/bin/Qt5MultimediaWidgets.dll"
		"${MINGW_PREFIX}/bin/Qt5Sql.dll"
		"${MINGW_PREFIX}/bin/Qt5Widgets.dll"
		"${MINGW_PREFIX}/bin/Qt5PrintSupport.dll"
		"${MINGW_PREFIX}/bin/Qt5OpenGL.dll"
		"${MINGW_PREFIX}/bin/Qt5Concurrent.dll"
		"${MINGW_PREFIX}/bin/Qt5WebKit.dll"
        "${MINGW_PREFIX}/bin/Qt5WebKitWidgets.dll"
		"${MINGW_PREFIX}/bin/libexpat-1.dll"
		"${MINGW_PREFIX}/bin/libjpeg-8.dll"
		"${MINGW_PREFIX}/bin/libpng16-16.dll"
		"${MINGW_PREFIX}/bin/libstdc++-6.dll"
		"${MINGW_PREFIX}/bin/libtiff-5.dll"
		"${MINGW_PREFIX}/bin/libtiffxx-5.dll"
		"${MINGW_PREFIX}/bin/libturbojpeg-0.dll"
		"${MINGW_PREFIX}/bin/zlib1.dll"
		"${MINGW_PREFIX}/bin/libwinpthread-1.dll"
		"${MINGW_PREFIX}/bin/libsqlite3-0.dll"
		"${MINGW_PREFIX}/bin/libiconv-2.dll"
		"${MINGW_PREFIX}/bin/libGLESv2.dll"
		"${MINGW_PREFIX}/bin/libfreetype-6.dll"
		"${MINGW_PREFIX}/bin/libglib-2.0-0.dll"
		"${MINGW_PREFIX}/bin/libharfbuzz-0.dll"
		"${MINGW_PREFIX}/bin/libpcre16-0.dll"
		"${MINGW_PREFIX}/bin/libpcre2-16-0.dll"
		"${MINGW_PREFIX}/bin/libintl-8.dll"
		"${MINGW_PREFIX}/bin/libbz2-1.dll"
		"${MINGW_PREFIX}/bin/libcurl-4.dll"
        "${MINGW_PREFIX}/bin/libpcre-1.dll"
        "${MINGW_PREFIX}/bin/libpcre32-0.dll"
        "${MINGW_PREFIX}/bin/libidn-11.dll"
        "${MINGW_PREFIX}/bin/libssh2-1.dll"
        "${MINGW_PREFIX}/bin/libEGL.dll"
        "${MINGW_PREFIX}/bin/Qt5Qml.dll"
        "${MINGW_PREFIX}/bin/Qt5Quick.dll"
        "${MINGW_PREFIX}/bin/Qt5Positioning.dll"
        "${MINGW_PREFIX}/bin/Qt5Sensors.dll"
        "${MINGW_PREFIX}/bin/libwebp-7.dll"
        "${MINGW_PREFIX}/bin/libxml2-2.dll"
        "${MINGW_PREFIX}/bin/libxslt-1.dll"
        "${MINGW_PREFIX}/bin/libpoppler-qt5-1.dll"
        "${MINGW_PREFIX}/bin/libpoppler-68.dll"
        "${MINGW_PREFIX}/bin/liblcms2-2.dll"
        "${MINGW_PREFIX}/bin/libopenjpeg-1.dll"
        "${MINGW_PREFIX}/bin/libopenjp2-7.dll"
        "${MINGW_PREFIX}/bin/libgraphite2.dll"
        "${MINGW_PREFIX}/bin/libicudt57.dll"
        "${MINGW_PREFIX}/bin/libicuin57.dll"
        "${MINGW_PREFIX}/bin/libicuuc57.dll"
        "${MINGW_PREFIX}/bin/liblzma-5.dll"
         "${MINGW_PREFIX}/bin/libidn2-0.dll"
         "${MINGW_PREFIX}/bin/libunistring-2.dll"
		"images/sito.html"
		"../README.txt"
		"../CHANGELOG"
		DESTINATION .
		)
		INSTALL(FILES "${MINGW_PREFIX}/bin/libopenjp2-7.dll" DESTINATION . RENAME "libopenjp2.dll")
IF(WIN64)
    INSTALL(FILES 
        "${MINGW_PREFIX}/bin/libgcc_s_seh-1.dll"
        "${MINGW_PREFIX}/bin/libmariadb.dll"
        "${MINGW_PREFIX}/bin/libcrypto-1_1-x64.dll"
        "${MINGW_PREFIX}/bin/libssl-1_1-x64.dll"
        DESTINATION . )
ELSE ()
    INSTALL(FILES 
        "${MINGW_PREFIX}/bin/libgcc_s_sjlj-1.dll"
        "${MINGW_PREFIX}/bin/libmySQL.dll"
        "${MINGW_PREFIX}/bin/libcrypto-1_1.dll"
        "${MINGW_PREFIX}/bin/libssl-1_1.dll"
        DESTINATION . )
ENDIF(WIN64)
	INSTALL(FILES 
		"${MINGW_PREFIX}/lib/qt/plugins/sqldrivers/qsqlmysql.dll" DESTINATION ./sqldrivers
		)
	INSTALL(FILES 
		"${MINGW_PREFIX}/lib/qt/plugins/platforms/qminimal.dll" 
		"${MINGW_PREFIX}/lib/qt/plugins/platforms/qwindows.dll"
		DESTINATION ./platforms
		)
	INSTALL(FILES 
		"${MINGW_PREFIX}/lib/qt/plugins/audio/qtaudio_windows.dll" 
		DESTINATION ./audio
		)
	INSTALL(FILES
		"${MINGW_PREFIX}/lib/qt/plugins/imageformats/qgif.dll"
		"${MINGW_PREFIX}/lib/qt/plugins/imageformats/qico.dll"
		"${MINGW_PREFIX}/lib/qt/plugins/imageformats/qjpeg.dll"
		"${MINGW_PREFIX}/lib/qt/plugins/imageformats/qsvg.dll"
		DESTINATION ./imageformats
		)
	INSTALL(FILES 
		"${MINGW_PREFIX}/lib/qt/plugins/printsupport/windowsprintersupport.dll" 
		DESTINATION ./printsupport
		)
    INSTALL(DIRECTORY
        "${MINGW_PREFIX}/lib/qt/qml/QtQuick/"
        DESTINATION ./QtQuick)
    INSTALL(DIRECTORY
        "${MINGW_PREFIX}/lib/qt/qml/QtQuick.2/"
        DESTINATION ./QtQuick.2)
	INSTALL(FILES
		"../windows/gunzip.exe"
		"../windows/gzip.exe"
		"../windows/unzip.exe" 
		DESTINATION ./windows )
	INSTALL(FILES
		"installdb/install_db.sql"
		DESTINATION ./installdb )
	INSTALL(FILES
		"updatedb/update_db.sql"
		DESTINATION ./updatedb )
ELSE()
    INSTALL( TARGETS FabariaGest RUNTIME DESTINATION /opt/FabariaGest)
    INSTALL( FILES "fabaria" DESTINATION /usr/bin PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE)
    INSTALL( FILES "images/logo4.png" DESTINATION /opt/FabariaGest)
    INSTALL( FILES "desktop/fabaria.desktop" DESTINATION /usr/share/applications )
    INSTALL(FILES
		"installdb/install_db.sql"
		DESTINATION /opt/FabariaGest/installdb )
	INSTALL(FILES
		"updatedb/update_db.sql"
		DESTINATION /opt/FabariaGest/updatedb )
endif()
