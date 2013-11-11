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
# Project specification file for datetimesettingsplugin.
#

TEMPLATE = lib
TARGET = datetimesettingsplugin
CONFIG += hb plugin

DEPENDPATH += . \
			  ./inc \
			  ./src
			  
INCLUDEPATH += . \
				../../../inc \
				../../../clockmw/clocktimezone/inc \
				../../clocksettingsview/inc \
				./traces

LIBS += \
		-lcpframework \
		-ltimezoneclient \
		-lclocksettingsview \

HEADERS += \
			datetimesettingsplugin.h \
			datetimesettingsview.h
		   
SOURCES += \
			datetimesettingsplugin.cpp \
			datetimesettingsview.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1  
    TARGET.UID3 = 0x102818E9
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	
    deploy.path = C:
    headers.sources = qmakepluginstubs/datetimesettingsplugin.qtplugin
    headers.path = /resource/qt/plugins/controlpanel

    # This is for new exporting system coming in garden
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "./$$header $$deploy.path$$headers.path/$$basename(header)"
}

plugin.sources = datetimesettingsplugin.dll
plugin.path = \resource\qt\plugins\controlpanel
DEPLOYMENT += plugin

# End of file	--Don't remove this.
