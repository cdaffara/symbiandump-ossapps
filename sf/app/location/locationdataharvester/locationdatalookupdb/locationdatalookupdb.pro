#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Location data lookup db project file

TEMPLATE = lib
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ..\inc


# By default Qt adds dependencies to QtCore and QtGui, 
# QtCore is enough for this example 
#QT = core
QT           += sql

#BUILD_DLL macro is used to define export macro
DEFINES += BUILD_DLL           

           
SOURCES += locationdatalookupdb.cpp

symbian: {
    TARGET.UID3 = 0x2002E714
    TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = ALL \
        -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}    

defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/locationdatalookupdb.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/locationdatalookupdb.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock



symbian:MMP_RULES += SMPSAFE
