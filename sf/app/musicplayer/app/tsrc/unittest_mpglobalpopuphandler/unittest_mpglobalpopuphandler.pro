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
# Description: mpglobalpopuphandler unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpglobalpopuphandler
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll \

HEADERS += inc/unittest_mpglobalpopuphandler.h \
		   ../../inc/mpglobalpopuphandler.h \	
		   stub/inc/mpmtpinfolink.h \
		   stub/inc/mpsongscanner.h \
		   stub/inc/mpenginefactory.h \
		   stub/inc/mpsettingsmanager.h \
	       stub/inc/mpengine.h \
               
SOURCES += src/unittest_mpglobalpopuphandler.cpp \ 		   
		   #../../src/mpglobalpopuphandler.cpp \
		   stub/src/mpmtpinfolink.cpp \
		   stub/src/mpsongscanner.cpp \
		   stub/src/mpenginefactory.cpp \
		   stub/src/mpsettingsmanager.cpp \
		   stub/src/mpengine.cpp \
	       
	       