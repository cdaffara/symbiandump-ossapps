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
#

TEMPLATE = app
TARGET = 
DEPENDPATH += .
CONFIG += hb
INCLUDEPATH += . \
    ../../inc
win32:INCLUDEPATH += c:/hb/include/hbcore \
    c:/hb/include/hbwidgets

symbian {    
TARGET.EPOCHEAPSIZE = 0x1000 0xA00000
INCLUDEPATH += . \
    /sf/app/phone/inc
TARGET.CAPABILITY = CAP_APPLICATION    
}

# Input
HEADERS += bubbletestview.h
SOURCES += main.cpp \
    bubbletestview.cpp
RESOURCES += bubbletest2.qrc

# Libraries
unix:!symbian LIBS += -L../../bubblecore
LIBS += -lbubblemanager2


symbian:MMP_RULES += SMPSAFE
