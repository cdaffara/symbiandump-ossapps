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
CONFIG += plugin
CONFIG += hb
TARGET = $$qtLibraryTarget(cntexampleactionsplugin)
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += \
    inc/cntexampleactionfactory.h \
    inc/cntexamplebaseaction.h \
    inc/cntexampleaction.h \
    inc/cntexample2action.h \
    inc/cntexamplenodetailaction.h \
    inc/cntexampledynamicaction.h
    
SOURCES += \
    src/cntexampleactionfactory.cpp \
    src/cntexamplebaseaction.cpp \
    src/cntexampleaction.cpp \
    src/cntexample2action.cpp \
    src/cntexamplenodetailaction.cpp \
    src/cntexampledynamicaction.cpp
MOC_DIR = moc

symbian:
 { 
    load(data_caging_paths)
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0xefa11113
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    LIBS += -lQtContacts \
        -lxqservice
    target.path = /sys/bin
    INSTALLS += target
    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbianplugin
}
symbian:MMP_RULES += SMPSAFE