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
# Description: This is the project specification file for the
# clockwidgetplugin.
#

TEMPLATE = lib
TARGET = clockwidgetplugin

win32{
CONFIG(release, debug|release){
	TARGET = clockwidgetplugin
	} else {
	TARGET = clockwidgetplugind
	}
}

CONFIG += hb plugin

win32{
CONFIG(release, debug|release){
		DESTDIR = ../release # for easy plugin loading
		LIBS += -L../release -lclockwidget
	} else {
		DESTDIR = ../debug # for easy plugin loading
		LIBS += -L../debug -lclockwidgetd
	}
}

SOURCES += \
        main.cpp

INCLUDEPATH += \
            ../clockwidgetimpl \ 
            ../clockwidgetimpl/inc

symbian: { 
    TARGET.UID3 = 0x2002E716
	TARGET.EPOCALLOWDLLDATA=1
	TARGET.CAPABILITY = CAP_GENERAL_DLL
	  
	LIBS += \
	    -lclockwidget.dll

	pluginstub.sources = clockwidgetplugin.dll
	pluginstub.path = /resource/plugins
	DEPLOYMENT += pluginstub
	DESTDIR = $$HB_PLUGINS_DIR
}

!local {
    target.path = $$HB_PLUGINS_DIR
    INSTALLS += target
}

# End of file	--Don't remove this.
