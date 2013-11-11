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
TARGET = secondpluginforcpcategorymodel

MOC_DIR = moc
symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}

CONFIG += hb plugin
LIBS += -lcpframework
CONFIG += symbian_test

symbian: plugin { # copy qtstub and manifest
	
	PLUGIN_STUB_PATH = /resource/qt/plugins/controlpanel
	
	deploy.path = C:
	pluginstub.sources = $${TARGET}.dll
	pluginstub.path = $$PLUGIN_STUB_PATH
	DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
     
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin  $$deploy.path$$qtplugins.path/$$basename(qtplugin)" 
}

DEPENDPATH += .
INCLUDEPATH += .
			 
# Input
HEADERS += src/secondpluginforcpcategorymodel.h \
           src/secondpluginviewforcpcategorymodel.h
SOURCES += src/secondpluginforcpcategorymodel.cpp \
           src/secondpluginviewforcpcategorymodel.cpp



symbian:MMP_RULES += SMPSAFE
