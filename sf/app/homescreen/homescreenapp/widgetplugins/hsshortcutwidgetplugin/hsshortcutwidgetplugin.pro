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
#

TEMPLATE = lib
CONFIG += plugin hb mobility
MOBILITY = serviceframework

HB += hbfeedback

WIDGET_SUBDIR = /private/20022F35/import/widgetregistry/20022F46
PLUGIN_SUBDIR = /resource/qt/plugins/homescreen

include(../../common.pri)

RESOURCES += hsshortcutwidgetplugin.qrc

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

LIBS += -lhsdomainmodel \
        -lcaclient

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \                
                ../../hsdomainmodel/inc

symbian: {
    TARGET.UID3 = 0x20022F46
    MMP_RULES += SMPSAFE
}

exportResources(./resource/*.manifest, $$WIDGET_SUBDIR)
