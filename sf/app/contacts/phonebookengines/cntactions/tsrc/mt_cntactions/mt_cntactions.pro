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

QT += testlib sql xml
CONFIG  += qtestlib
CONFIG  += symbian_test
#DEFINES += PBK_UNIT_TEST

DEPENDPATH += .
INCLUDEPATH += .

symbian:
{
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
	
	# Input
	HEADERS += testrunner.h
	HEADERS += mt_cntactions.h
	
	SOURCES += main.cpp
	SOURCES += testrunner.cpp
	SOURCES += mt_cntactions.cpp
	
	TARGET.CAPABILITY = ALL -TCB
  
  LIBS += -lQtContacts 
  
}
symbian:MMP_RULES += SMPSAFE
