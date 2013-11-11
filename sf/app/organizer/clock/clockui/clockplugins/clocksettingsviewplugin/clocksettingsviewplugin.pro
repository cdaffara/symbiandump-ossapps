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
# Project specification file for clocksettingsviewplugin.
#

TEMPLATE = lib
TARGET = clocksettingsviewplugin
CONFIG += hb plugin

DEPENDPATH += . \
			  ./inc \
			  ./src

INCLUDEPATH += . \
				../../../inc \
				../../clocksettingsview/inc \
				../../../clockmw/clocksettingsutility/inc \
				./traces

LIBS += \
		-lclocksettingsview

HEADERS += \
			clocksettingsviewplugin.h \
			../../../../organizer_plat/clock_settingsview_plugin_api/inc/clocksettingsviewinterface.h
		   
SOURCES += \
			clocksettingsviewplugin.cpp

symbian: {
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	TARGET.EPOCALLOWDLLDATA = 1  
	TARGET.UID3 = 0x2003157C
	
	load(data_caging_paths)
	# plugin stub deployment

	plugin.sources = $${TARGET}.dll
	plugin.path = $$QT_PLUGINS_BASE_DIR/clock
	DEPLOYMENT += plugin

	# for target
	BLD_INF_RULES.prj_exports += \
	"./qmakepluginstubs/clocksettingsviewplugin.qtplugin  /$$HW_ZDIR$$RESOURCE_FILES_DIR/qt/plugins/clock/clocksettingsviewplugin.qtplugin"
}


# End of file	--Don't remove this.
