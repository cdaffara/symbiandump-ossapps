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
# Description:  search test online project file
#

TEMPLATE      	= lib
CONFIG       	+= plugin hb
TARGET        	= Baidu

plugin.sources 	= Baidu.dll
plugin.path 	= \resource\qt\plugins\onlinesearch
 
DEPLOYMENT 	+= plugin

CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

win32 {
    
    LIBS += -L$$PWD/../../../../bin/debug
}



CONFIG += plugin debug_and_release

QT += xml sql webkit network



coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH +=   ./inc \
                ./src

INCLUDEPATH += ./inc/ \	       
       
symbian: {
    TARGET.UID3 = 0x2002A532
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB -DRM
	INCLUDEPATH +=  $$APP_LAYER_SYSTEMINCLUDE
CONFIG += symbian_test
}    

include(t_baiduplugin.pri)
symbian:include(t_baiduplugin_installs_symbian.pri)
win32:include(t_baiduplugin_installs_win32.pri)
symbian:unix:include(t_baiduplugin_installs_unix.pri)
