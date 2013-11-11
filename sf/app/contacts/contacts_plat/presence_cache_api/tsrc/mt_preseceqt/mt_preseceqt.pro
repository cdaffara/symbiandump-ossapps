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
TARGET = mt_preseceqt
QT += testlib network webkit xmlpatterns
CONFIG += mobility
DEFINES += CNT_SOC_ENG_UNIT_TEST

TARGET.CAPABILITY = ALL -TCB

DEPENDPATH += .
INCLUDEPATH += .
load(data_caging_paths)

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH +=   ..\..\..\\presence_cache_api\inc
INCLUDEPATH +=   ..\..\inc

HEADERS +=    entitytests.h
    
SOURCES +=  start.cpp \ 
            entitytests.cpp 
    
QT += sql
 
LIBS += -lpresencecacheqt
symbian:MMP_RULES += SMPSAFE