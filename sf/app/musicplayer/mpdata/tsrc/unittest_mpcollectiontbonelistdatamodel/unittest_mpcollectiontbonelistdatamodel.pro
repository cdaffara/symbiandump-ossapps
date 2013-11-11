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
# Description: mpcollectiontbonelistdatamodel unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpcollectiontbonelistdatamodel
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll

HEADERS += inc/unittest_mpcollectiontbonelistdatamodel.h \
           inc/unittest_helper.h \
           stub/inc/mpmpxcollectiondata.h \
           stub/inc/mpplaybackdata.h \
           ../../../inc/mpcollectiontbonelistdatamodel.h 
               
SOURCES += src/unittest_mpcollectiontbonelistdatamodel.cpp \
           src/unittest_helper.cpp \
           stub/src/mpmpxcollectiondata.cpp \
           stub/src/mpplaybackdata.cpp \
           ../../src/mpcollectiontbonelistdatamodel.cpp

DEFINES += BUILD_MPDATA_LIB

