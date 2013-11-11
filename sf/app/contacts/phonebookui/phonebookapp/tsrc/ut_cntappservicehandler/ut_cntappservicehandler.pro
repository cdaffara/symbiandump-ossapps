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
TARGET =

QT += testlib
CONFIG  += qtestlib hb
DEFINES += PBK_UNIT_TEST

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../stubs

HEADERS += t_cntappservicehandler.h \
            ../../inc/cntappservicehandler.h
SOURCES += t_cntappservicehandler.cpp \
           qthighway_stub.cpp\
           ../../src/cntappservicehandler.cpp

# capability
TARGET.CAPABILITY = ALL -TCB

LIBS+=-lhbcore -lxqservice -lxqserviceutil
symbian:MMP_RULES += SMPSAFE