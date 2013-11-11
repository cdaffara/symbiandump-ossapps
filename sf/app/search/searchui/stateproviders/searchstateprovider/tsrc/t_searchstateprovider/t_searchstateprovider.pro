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
# Description:  Search stateprovider unit test project file
#
QT += testlib
TEMPLATE = app

DEFINES += BUILD_SEARCHSTATEPROVIDER
CONFIG += hb console mobility
MOBILITY = serviceframework


LIBS += -lsearchindevicehandler
LIBS += -lxqservice 
LIBS += -lcpixsearch
LIBS += -laknskins 
LIBS += -lfbscli
LIBS += -laknicon
LIBS += -lapgrfx
LIBS += -lbitgdi
LIBS += -lsearchonlinehandler
LIBS += -lxqutils
LIBS += -lapparc
LIBS += -lefsrv
LIBS += -lapgrfx
LIBS += -lws32
LIBS += -ltstaskmonitorclient
LIBS += -lcpixcontentinfodb
LIBS += -lxqserviceutil

CONFIG += qtestlib
CONFIG += hb

QT += xml \
      sql
DEFINES += SEARCH_UNITTESTING

DEPENDPATH += .\
              ./src \
              ./inc \
              ../../src \
              ../../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../../inc \               
               ../../../../indevicehandler/inc \
               ../../../../onlinehandler/inc
symbian {         
CONFIG += symbian_test               
TARGET.UID2 = 0x100039CE
TARGET.UID3 = 0x20026F9A
TARGET.CAPABILITY = CAP_GENERAL_DLL
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
}

include(t_searchstateprovider.pri)

