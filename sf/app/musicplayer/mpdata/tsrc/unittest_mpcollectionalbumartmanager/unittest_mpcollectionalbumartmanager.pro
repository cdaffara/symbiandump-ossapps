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
# Description: mpcollectionalbumartmanager unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpcollectionalbumartmanager
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll

HEADERS += inc/unittest_mpcollectionalbumartmanager.h \
           stub/inc/thumbnailmanager_qt.h \
           stub/inc/mpmpxcollectiondata.h \
           ../../inc/mpcollectionalbumartmanager.h \
               
SOURCES += src/unittest_mpcollectionalbumartmanager.cpp \
           stub/src/thumbnailmanager_qt.cpp \
           stub/src/mpmpxcollectiondata.cpp \
           ../../src/mpcollectionalbumartmanager.cpp

RESOURCES += resources/unittest_mpcollectionalbumartmanagerresources.qrc

