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
# Description: mpplaybackdata unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpplaybackdata
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
    	-lfbscli.dll \
				
HEADERS += inc/unittest_mpplaybackdata.h \        
           stub/inc/thumbnailmanager_qt.h \
           ../../../inc/mpplaybackdata.h 
               
SOURCES += src/unittest_mpplaybackdata.cpp \
           ../../src/mpplaybackdata.cpp \
           stub/src/thumbnailmanager_qt.cpp

DEFINES += BUILD_MPDATA_LIB

