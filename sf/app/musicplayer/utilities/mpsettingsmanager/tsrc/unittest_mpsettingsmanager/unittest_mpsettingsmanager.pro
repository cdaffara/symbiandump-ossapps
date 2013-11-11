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
# Description: mpsettingsmanager unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpsettingsmanager
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \ 
               ../../inc \
               ../../../../inc    
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += inc/unittest_mpsettingsmanager.h \
           stub/inc/xqsettingskey.h \
           stub/inc/xqsettingsmanager.h \
           ../../../../inc/mpsettingsmanager.h
           
SOURCES += src/unittest_mpsettingsmanager.cpp \
           stub/src/xqsettingskey.cpp \
           stub/src/xqsettingsmanager.cpp

           
DEFINES += BUILD_MPSETTINGSMANAGER

