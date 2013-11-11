# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = lib
CONFIG += hb mobility
MOBILITY += publishsubscribe
include (../../common.pri)
DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    inc
HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

DEFINES += MENUSERVICE_LIB

LIBS += -lcaclient \
        -lcasoftwareregistry

symbian::TARGET.UID3 = 0x20026FA9
!symbian:exportResources(./resource/*.png, resource)


symbian:MMP_RULES += SMPSAFE
