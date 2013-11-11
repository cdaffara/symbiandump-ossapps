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

MOC_DIR = moc

QT += testlib xml core
CONFIG  += qtestlib hb
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += PBK_UNIT_TEST
DEFINES += CNTHISTORYMODEL_NO_EXPORT

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
	
# Input
HEADERS += ../../inc/*.h
HEADERS += mt_cnthistorymodel.h
HEADERS += stub_classes.h
HEADERS += testrunner.h
           
SOURCES += ../../src/*.cpp
SOURCES += main.cpp
SOURCES += stub_classes.cpp
SOURCES += mt_cnthistorymodel.cpp
SOURCES += testrunner.cpp
           
LIBS += -lQtContacts \
        -lmsghistory

TARGET.CAPABILITY = ALL -TCB
symbian:MMP_RULES += SMPSAFE