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
TARGET = $$qtLibraryTarget(cntactionsplugin)
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += \
    inc/cntactionfactory.h \
    inc/cntaction.h \
    inc/cntmessageaction.h \
    inc/cntvideocallaction.h \
    inc/cntcallaction.h \
    inc/cntemailaction.h \
    inc/cntbrowseraction.h
    
SOURCES += \
    src/cntactionfactory.cpp \
    src/cntaction.cpp \
    src/cntmessageaction.cpp \
    src/cntvideocallaction.cpp \
    src/cntcallaction.cpp \
    src/cntemailaction.cpp \
    src/cntbrowseraction.cpp

MOC_DIR = moc

symbian:
 { 
    load(data_caging_paths)
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20027012
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