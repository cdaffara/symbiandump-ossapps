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
CONFIG += qtestlib \
					symbian_test \
					hb
QT += webkit \
    network \
    xml

TARGET.CAPABILITY = All -TCB
TARGET = 
DEPENDPATH += . 
INCLUDEPATH += ./stub/inc \
	       ../../inc \
	       ../../../../inc
	       

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

LIBS += -lmpxviewframeworkqt.dll \
        -lmpsettingsmanager.dll

MOC_DIR = ./

# Input
HEADERS += inc/unittest_mpdetailsview.h \
		   ../../inc/mpdetailsview.h \
		   stub/inc/mpquerymanager.h \
		   stub/inc/mpengine.h \
		   stub/inc/mpenginefactory.h \
           stub/inc/mpsongdata.h \
           stub/inc/thumbnailmanager_qt.h \
           ../../../../inc/mpviewbase.h
           

SOURCES += src/unittest_mpdetailsview.cpp \
               #../../src/mpdetailsview.cpp \
		   stub/src/mpquerymanager.cpp \	       
		   stub/src/mpengine.cpp \
		   stub/src/mpenginefactory.cpp \		       
           stub/src/mpsongdata.cpp \
           stub/src/thumbnailmanager_qt.cpp \

RESOURCES += ../../resources/mpdetailsviewresources.qrc
