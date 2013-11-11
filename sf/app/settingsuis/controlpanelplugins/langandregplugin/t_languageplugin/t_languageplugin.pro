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
# Description: tests for Language plugin
#

TEMPLATE = app

CONFIG += debug_and_release
QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
} else {
    DESTDIR = ./release
}

symbian: LIBS+=-lcpframework 
LIBS += -lstarterclient

CONFIG += hb 

INCLUDEPATH += ./inc \
               ../inc                


DEPENDPATH += ./inc \
              ./src \
              ../inc \
              ../src 

DEFINES += LANGUAGEPLUGIN_TEST 


symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002BCD6
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    PLUGIN_SUBDIR = c:/private/2002BCD6
}
symbian:MMP_RULES += SMPSAFE
include( t_languageplugin.pri )

