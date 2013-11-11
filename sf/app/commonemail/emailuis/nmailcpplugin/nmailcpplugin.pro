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

TARGET = nmailcpplugin

QT += core

DEPENDPATH += .

INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb
CONFIG += plugin

MOC_DIR = moc
OBJECTS_DIR = obj
RCC_DIR = rcc

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
                    ARMV5

# Input
HEADERS += inc/nmsettingsplugin.h

SOURCES += src/nmsettingsplugin.cpp

symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    DEFINES += PLUGINUID3=0x2002BCF6
    TARGET.UID3 = 0x2002BCF6
    
    LIBS += -lcpframework
    LIBS += -lnmailuiengine
    LIBS += -lnmailbase
    LIBS += -lnmsettingui
    LIBS += -leuser
    LIBS += -llibc
    
    deploy.path = C:
    qtplugins.path = /resource/qt/plugins/controlpanel
    qtplugins.sources += qmakepluginstubs/nmailcpplugin.qtplugin
    
    # This is for new exporting system coming in garden
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"

    # Build.inf rules
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/nmailcpplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(nmailcpplugin.iby)"

    MMP_RULES += SMPSAFE
}

TARGET.CAPABILITY = All -TCB
plugin.sources = nmailcpplugin.dll
plugin.path = /resource/qt/plugins/controlpanel
DEPLOYMENT += plugin

