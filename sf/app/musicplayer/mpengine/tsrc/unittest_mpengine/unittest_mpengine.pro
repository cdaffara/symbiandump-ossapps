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
# Description: mpengine unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpengine
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor.dll \
        -lfbscli.dll \

HEADERS += inc/unittest_mpengine.h \
		   ../../../inc/mpengine.h \
           stub/inc/mpmpxharvesterframeworkwrapper.h \
		   stub/inc/mpsongdata.h \
		   stub/inc/mpsongscanner.h \
		   stub/inc/mpmediakeyhandler.h \     
		   stub/inc/mpmpxcollectionframeworkwrapper.h \
		   stub/inc/mpmpxcollectiondata.h \
		   stub/inc/mpmpxplaybackframeworkwrapper.h \
		   stub/inc/mpplaybackdata.h \
		   stub/inc/mpaudioeffectsframeworkwrapper.h \
		   stub/inc/mpequalizerframeworkwrapper.h \
	       stub/inc/mpsettingsmanager.h \
               
SOURCES += src/unittest_mpengine.cpp \ 		   
		   stub/src/mpmpxharvesterframeworkwrapper.cpp \ 
           stub/src/mpsongdata.cpp \
           stub/src/mpsongscanner.cpp \    
		   stub/src/mpmediakeyhandler.cpp \
		   stub/src/mpmpxcollectionframeworkwrapper.cpp \
		   stub/src/mpmpxcollectiondata.cpp \
		   stub/src/mpmpxplaybackframeworkwrapper.cpp \
		   stub/src/mpplaybackdata.cpp \
		   stub/src/mpaudioeffectsframeworkwrapper.cpp \
	       stub/src/mpequalizerframeworkwrapper.cpp \
	       stub/src/mpsettingsmanager.cpp \
	       
DEFINES += BUILD_MPENGINE_LIB	       
	       