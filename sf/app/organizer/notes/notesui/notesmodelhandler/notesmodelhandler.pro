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
# Description: This is the project specification file for the
# notesappcontroller component.
#

TEMPLATE = lib
CONFIG += hb
TARGET = notesmodelhandler
DEFINES += NOTESMODELHANDLER_LIB

DEPENDPATH += \
		./inc \
		./src

INCLUDEPATH += \
		../notesmodel/inc \
		../../inc \
		./traces

symbian: {
	TARGET.CAPABILITY = ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x2002BCDC
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
	LIBS += \
		-lnotesmodel \
		-lagendainterface
}

SOURCES += \
		notesmodelhandler.cpp

HEADERS += \
		notesmodelhandler.h \
		notesmodelhandlerdefines.h

# End of file	--Don't remove this.
