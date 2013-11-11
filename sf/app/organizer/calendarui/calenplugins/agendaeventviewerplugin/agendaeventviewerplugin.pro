#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
# Project specification file for Agenda Event Viewer.
#

TEMPLATE = lib
CONFIG += plugin
CONFIG += hb
TARGET = $$qtLibraryTarget(agendaeventviewerplugin)

INCLUDEPATH += \
		. \
		./src \
		./inc \
		../../inc
		
		
		
DEPENDPATH += \
		. \
		./src \
		./inc

symbian: {

			
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002E6A0 
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE	
	
	LIBS += -lagendainterface \
			-lagendaeventviewer


load(data_caging_paths)
# plugin stub deployment

plugin.sources = $${TARGET}.dll
plugin.path = $$QT_PLUGINS_BASE_DIR/calendar
DEPLOYMENT += plugin

# for target
BLD_INF_RULES.prj_exports += \
"./qmakepluginstubs/agendaeventviewerplugin.qtplugin  /$$HW_ZDIR$$RESOURCE_FILES_DIR/qt/plugins/calendar/agendaeventviewerplugin.qtplugin" 

}

SOURCES += agendaeventviewerplugin.cpp
		
HEADERS += agendaeventviewerplugin.h \
           ../../../organizer_plat/eventviewer_plugin_api/inc/eventviewerplugininterface.h

# End of file	--Don't remove this.
