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
# Description: mpsongdata unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpsongdata
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += . 
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxcommon.dll

HEADERS += inc/unittest_mpsongdata.h \
	   ../../../inc/mpsongdata.h \
	   ../../inc/mpsongdata_p.h \
	   stub/inc/thumbnailmanager_qt.h 
	   
SOURCES += src/unittest_mpsongdata.cpp \
	   ../../src/mpsongdata.cpp \
	   stub/src/thumbnailmanager_qt.cpp

DEFINES += BUILD_MPDATA_LIB

