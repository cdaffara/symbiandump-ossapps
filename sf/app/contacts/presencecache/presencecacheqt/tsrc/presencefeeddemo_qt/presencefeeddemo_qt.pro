#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).

#
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
TARGET = presencefeeddemo
CONFIG += hb

DEPENDPATH += .
INCLUDEPATH += inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

LIBS += -lqtcontacts \
        -lpresencecacheqt

# Input
HEADERS += inc/feedview.h

SOURCES += src/main.cpp
SOURCES += src/feedview.cpp

RESOURCES += presencefeeddemo.qrc

# capability
TARGET.CAPABILITY = CAP_APPLICATION
    
symbian: {
        TARGET.EPOCSTACKSIZE = 0x14000
        TARGET.EPOCHEAPSIZE = 0x1000 0xA00000 
}