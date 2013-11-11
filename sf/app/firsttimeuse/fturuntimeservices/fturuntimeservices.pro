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
# Description:  FTU runtime services project file
#

TEMPLATE = lib

symbian {
    CONFIG(debug, debug|release) {
      DESTDIR = debug
    } else {
      DESTDIR = release
    }
}

win32 {
    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }
    DESTDIR = $$PWD/../../bin/$$SUBDIRPART
}
CONFIG += debug_and_release

DEFINES += FTURUNTIMESERVICES_LIB

DEPENDPATH += . \
              inc \
              src

INCLUDEPATH += . \
               inc

CONFIG += hb

symbian {
TARGET.UID3 = 0x20027037
TARGET.EPOCALLOWDLLDATA=1
TARGET.CAPABILITY = ALL -TCB
LIBS += -lswinstcli
BLD_INF_RULES.prj_exports += "./inc/fturuntimeservices_global.h |../inc/" \                             
                             "./inc/ftucontentservice.h |../inc/"
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

#Add default symbian libs required by the application (redundant/duplicate entries to be removed)
LIBS += -lcone -leikcore -lmediaclientaudio -leikcoctl -leiksrv -lapparc -lavkon
LIBS += -lefsrv -lcharconv -lws32 -lhal -lgdi -lapgrfx

}

win32 {
}
include(fturuntimeservices.pri)
system(copy /y .\\inc\\fturuntimeservices_global.h ..\\inc)
system(copy /y .\\inc\\ftucontentservice.h ..\\inc)

#ONLY FOR DEVELOPMENT! REMOVE THIS BEFORE EACH HS_Domain RELEASE!



symbian:MMP_RULES += SMPSAFE
