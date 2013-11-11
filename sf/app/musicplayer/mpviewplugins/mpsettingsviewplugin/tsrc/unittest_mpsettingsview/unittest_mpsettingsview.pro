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
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpsettingsview
TARGET.CAPABILITY = CAP_APPLICATION


DEPENDPATH += . 
INCLUDEPATH += ./stub/inc \
	       ../../inc \
	       ../../../../inc
	       

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lmpxviewframeworkqt.dll \
        -lmpsettingsmanager.dll


# Input
HEADERS += inc/unittest_mpsettingsview.h \
            ../../inc/mpsettingsview.h \
            stub/inc/mpsettingsaudioeffectswidget.h \
           ../../../../inc/mpviewbase.h
           

SOURCES += src/unittest_mpsettingsview.cpp \
               ../../src/mpsettingsview.cpp \
            stub/src/mpsettingsaudioeffectswidget.cpp \

RESOURCES += ../../resources/mpsettingsview.qrc
