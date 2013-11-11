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
HB += hbfeedback
TARGET = mpnowplayingbanner
symbian: { 
    TARGET.UID3 = 0x10207C67
    MMP_RULES += "DEFFILE mpnowplayingbanner.def" SMPSAFE
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
DEFINES += BUILD_MPNOWPLAYINGBANNER

INCLUDEPATH += . \
	 inc \
	 ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
     
LIBS += -lmpengine \
        -lmpdata \
        -lmpxcommon
        
# Input
HEADERS += ../../inc/mpnowplayingwidget.h \
           inc/mpnowplayingwidget_p.h 

SOURCES += src/mpnowplayingwidget.cpp \
           src/mpnowplayingwidget_p.cpp
     
RESOURCES += resources/mpnowplayingbanner.qrc

DOCML += resources/nowplaying.docml
