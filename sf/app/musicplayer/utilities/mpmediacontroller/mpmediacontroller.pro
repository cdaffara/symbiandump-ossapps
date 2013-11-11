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

TEMPLATE = lib
CONFIG += hb qt
TARGET = mpmediacontroller
symbian: { 
    TARGET.UID3 = 0x10207C97
    MMP_RULES += "DEFFILE mpmediacontroller.def"
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
DEFINES += BUILD_MPMEDIACONTROLLER

INCLUDEPATH += . \
      inc \
      ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
     
LIBS += -lmpengine \
        -lremconcoreapi \
        -lremconinterfacebase

# Input
HEADERS += ../../inc/mpmediacontroller.h \
           inc/mpmediakeyhandler.h \
           inc/mpmediakeyhandler_p.h \
           inc/mpmediakeyremconresponse.h \
           inc/mpvolumeslider.h

SOURCES += src/mpmediacontroller.cpp \
           src/mpmediakeyhandler.cpp \
           src/mpmediakeyhandler_p.cpp \
           src/mpmediakeyremconresponse.cpp \
           src/mpvolumeslider.cpp

