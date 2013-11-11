#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Nokia Corporation - initial contribution.
# 
# Contributors: 
# 
# Description: 
# This is the project specification file for the notes editor plugin.
#

TEMPLATE = lib
CONFIG += hb plugin
TARGET = noteseditorplugin

INCLUDEPATH += \
		. \
		./src \
		./inc \
		../../../inc \
		./traces
		
DEPENDPATH += \
		. \
		./src \
		./inc

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002FF88

	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE	

	LIBS += \
		-lagendainterface \
		-lnoteseditor

load(data_caging_paths)

plugin.sources = $${TARGET}.dll
plugin.path = $$QT_PLUGINS_BASE_DIR/notes
DEPLOYMENT += plugin

BLD_INF_RULES.prj_exports += \
"./qmakepluginstubs/noteseditorplugin.qtplugin  /$$HW_ZDIR$$RESOURCE_FILES_DIR/qt/plugins/notes/noteseditorplugin.qtplugin" 
}

HEADERS += \
		../../../../organizer_plat/notes_editor_api/inc/noteseditorinterface.h \
		noteseditorplugin.h

SOURCES += noteseditorplugin.cpp

# End of file	--Don't remove this.
