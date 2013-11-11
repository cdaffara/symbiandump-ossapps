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
#
#

TEMPLATE = lib
TARGET = PerfMonDataPopupPlugin
CONFIG += plugin hb

# directories
INCLUDEPATH += inc
DEPENDPATH += inc

HEADERS += inc/perfmondatapopupplugin_p.h
HEADERS += inc/perfmondatapopupdialog_p.h
HEADERS += inc/perfmondatapopupwidget_p.h

SOURCES += src/perfmondatapopupplugin.cpp
SOURCES += src/perfmondatapopupdialog.cpp
SOURCES += src/perfmondatapopupwidget.cpp

symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002E6B0

    hblib.sources = Hb.dll
    hblib.path = /sys/bin
    hblib.depends = "(0xEEF9EA38), 1, 0, 0, {\"Hb\"}"

    pluginstub.sources = PerfMonDataPopupPlugin.dll
    pluginstub.path = /resource/plugins/devicedialogs/
    DEPLOYMENT += pluginstub
}