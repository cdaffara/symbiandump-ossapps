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

TEMPLATE = app
TARGET = 
DEPENDPATH += . 
              
INCLUDEPATH += . 
INCLUDEPATH += ../../../ui/uiengine/model/mediamodel/inc
INCLUDEPATH += ../../../ui/uiengine/medialistwrapper/inc
INCLUDEPATH += ../../../ui/inc
INCLUDEPATH += ../../../inc

INCLUDEPATH += /epoc32/include/mw/QtTest

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

               
CONFIG += qtestlib \
          Hb

symbian: { 
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x1600000
    TARGET.UID3 = 0x200009EE
    MMP_RULES += SMPSAFE 
}

LIBS += -lflogger.dll 
LIBS += -lglxmediamodel.dll
LIBS += -lglxmedialistwrapper.dll

# Input
HEADERS += unittest_mediamodel.h
          
SOURCES += unittest_mediamodel.cpp

myFiles.sources = "../../../gallery/data/Battle.jpg"
myFiles.sources += "../../../gallery/data/abstract5.jpg"
myFiles.sources += "../../../gallery/data/newitem1.jpg"
myFiles.sources += "../../../gallery/data/newitem2.jpg"
myFiles.sources += "../../../gallery/data/newitem3.jpg"
myFiles.path = c:\data\Images
DEPLOYMENT += myFiles
