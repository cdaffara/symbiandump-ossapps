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
TARGET = videoservicestestapp

CONFIG += hb
LIBS += -lxqservice -lxqserviceutil

HEADERS += inc/videoservicestestappview.h 

SOURCES += src/videoservicestestappview.cpp \
           src/main.cpp


symbian {
    TARGET.UID2 = 0x100039CE 
    TARGET.UID3 = 0x2002C341
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
    
    BLD_INF_RULES.prj_exports += "rom/videoservicestestapp.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videoservicestestapp.iby)"
}
