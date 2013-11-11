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
# Description: mptracklistwidget unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mptracklistwidget
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
			   ../../inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += inc/unittest_mptracklistwidget.h \
		   ../../inc/mptracklistwidget.h \
		   stub/inc/hbinstancestub.h \	
		   stub/inc/hbmainwindownstub.h \


SOURCES += src/unittest_mptracklistwidget.cpp \
	 	   stub/src/hbinstancestub.cpp \	

DEFINES += UNIT_TESTING