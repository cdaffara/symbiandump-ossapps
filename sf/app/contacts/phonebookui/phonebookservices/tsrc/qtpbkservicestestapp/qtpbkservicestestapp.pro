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
# Description: App for testing Contacts highway UI services
#

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../../../inc

CONFIG += hb

LIBS+=-lhbcore -lxqservice -lqtcontacts -lcntlistmodel

# Input
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += testpbkservices.h

SOURCES += main.cpp \
           testpbkservices.cpp

TARGET.CAPABILITY = ALL -TCB

BLD_INF_RULES.prj_exports += \
        "testvcard.vcf \epoc32\winscw\c\data\others\testvcard.vcf"
symbian:MMP_RULES += SMPSAFE        