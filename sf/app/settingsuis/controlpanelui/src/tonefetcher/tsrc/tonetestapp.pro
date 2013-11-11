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
TARGET = tonetestapp 

CONFIG += hb
LIBS += -lxqservice -lxqserviceutil


HEADERS   += tonetestapp.h
SOURCES   += tonetestapp_reg.rss \
    main.cpp \
    tonetestapp.cpp
RESOURCES +=
symbian {
    include(rom/rom.pri)
    TARGET.UID3 = 0x2002BCC9
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
}



symbian:MMP_RULES += SMPSAFE
