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
# Description: mpmediakeyhandler unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib symbian_test
TARGET = unittest_mpmediakeyhandler
TARGET.CAPABILITY = CAP_APPLICATION

symbian: { 
    MMP_RULES += SMPSAFE
}

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lestor \
        -lfbscli \
        -lremconcoreapi \
        -lremconinterfacebase

HEADERS += stub/inc/mpenginefactory.h \
           stub/inc/remconcoreapitarget.h \
           stub/inc/remconcoreapitargetobserver.h \
           stub/inc/remconinterfaceselector.h \
           inc/unittest_mpmediakeyhandler.h \
           ../../inc/mpmediakeyhandler.h \
           ../../inc/mpmediakeyhandler_p.h \
           ../../inc/mpmediakeyremconresponse.h

SOURCES += stub/src/mpenginefactory.cpp \
           stub/src/remconcoreapitarget.cpp \
           stub/src/remconinterfaceselector.cpp \
           src/unittest_mpmediakeyhandler.cpp \
           ../../src/mpmediakeyhandler.cpp

