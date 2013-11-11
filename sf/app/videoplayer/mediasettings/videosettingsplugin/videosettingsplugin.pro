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
CONFIG += hb plugin
TARGET = videosettingsplugin

symbian: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002BC69
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    BLD_INF_RULES.prj_exports += "rom/videosettingsplugin.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(videosettingsplugin.iby)"
    MMP_RULES += SMPSAFE
    VERSION = 10.0
}

LIBS += -lcpframework \
        -lMPSettEngine.dll \
        -lxqutils.dll \
        -lcmapplsettingsui \
        -lconnection_settings_shim

INCLUDEPATH += ../mediasettingsengine/inc \
               ../../inc

HEADERS += inc/videosettingsplugin.h \
           inc/videosettingsgroup.h \
           inc/videosettingsaccesspointentry.h 
    
SOURCES += src/videosettingsplugin.cpp \
           src/videosettingsgroup.cpp \
           src/videosettingsaccesspointentry.cpp

deploy.path = C:
qtplugins.path = /resource/qt/plugins/controlpanel
qtplugins.sources += qmakepluginstubs/videosettingsplugin.qtplugin

for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"

plugin.sources += videosettingsplugin.dll
plugin.path = /resource/qt/plugins/controlpanel
DEPLOYMENT += plugin
