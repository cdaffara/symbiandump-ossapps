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
# Description: Project definition file for notesmodel.
# 

TEMPLATE = lib
TARGET = notesmodel
CONFIG += hb
DEFINES += NOTESMODEL_LIB

INCLUDEPATH += \
		. \
		../notesviewmanager/inc \
		../inc \
		../../inc \
		./traces

DEPENDPATH += \
		. \
		./inc \
		./src

symbian: {
	TARGET.CAPABILITY += ALL -TCB
	TARGET.EPOCALLOWDLLDATA = 1
	TARGET.UID3 = 0x20029F81
	
	INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

	LIBS += \
		-lagendainterface
}

SOURCES += \
		notesmodel.cpp \
		notessortfilterproxymodel.cpp

HEADERS += \
		notesmodel.h \
		notessortfilterproxymodel.h \
		notesmodeldefines.h

# End of file	--Don't remove this.
