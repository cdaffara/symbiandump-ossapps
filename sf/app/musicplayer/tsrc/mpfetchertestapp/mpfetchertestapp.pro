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
TARGET = mpfetchertestapp

CONFIG += hb
LIBS += -lxqservice \
        -lxqserviceutil \
        -lapgrfx \
        -lcone

HEADERS += inc/mpfetchertestappview.h 

SOURCES += src/mpfetchertestappview.cpp \
           src/main.cpp


symbian {
#   include(rom/rom.pri) # to include in image
    TARGET.UID2 = 0x100039CE 
    TARGET.UID3 = 0x10207C6A
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x1F00000
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
}
