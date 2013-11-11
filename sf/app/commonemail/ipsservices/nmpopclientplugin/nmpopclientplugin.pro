# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

CONFIG += debug
CONFIG += plugin
CONFIG += hb
TEMPLATE = lib
TARGET = nmpopclientplugin
QT += core 
    
DEFINES += BUILD_CLIENTPLUGIN_DLL

INCLUDEPATH += inc \
    ../inc \
    ../../inc 

HEADERS += inc/nmpopclientplugin.h \
    inc/nmpopclientpluginheaders.h

SOURCES += src/nmpopclientplugin.cpp

LIBS += -leuser
LIBS += -llibc

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002B3D2

    PLUGIN_STUB_PATH = /resource/qt/plugins/nmail/uiext
    
    deploy.path = C:
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_STUB_PATH
    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)" 
 
    DEPLOYMENT += pluginstub
    LIBS += -lnmailbase \
            -lnmsettingui \
            -lnmailuiengine
            
    MMP_RULES += SMPSAFE
}

win32 {    
    INCLUDEPATH += ../ipssosplugin/inc
    DESTDIR = ../../bin
    pluginstub.sources = nmpopclientplugin.dll
    pluginstub.path = DESTDIR
    DEPLOYMENT += pluginstub
    LIBS += -L../../bin \
                -lnmailbase \
                -lnmsettingui \
                -lnmailuiengine 
}

plugin.sources = $${TARGET}.dll
plugin.path = $$PLUGIN_STUB_PATH
DEPLOYMENT += plugin

