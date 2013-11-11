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

include (../common.pri)

CONFIG += hb mobility
MOBILITY = systeminfo
QT += sql

DEFINES += HSUTILS_LIB
DEPENDPATH += . \
              inc \
              src

INCLUDEPATH += . \
               inc \
            ../serviceproviders/hsmenuserviceprovider/inc

symbian: {
    TARGET.UID3 = 0x20022F3C
    LIBS+=-lxqservice -lxqserviceutil
    
    MMP_RULES += SMPSAFE
}

include(hsutils.pri)


