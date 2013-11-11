#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).


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

TEMPLATE = lib
TARGET = geocodeupdate   

CONFIG += dll
CONFIG += Qt

DEPENDPATH += .


# Input
# Input
HEADERS += ./inc/geocodeupdate.h \
           ../mylocationlogger/inc \
           ../inc   
SOURCES +=./src/geocodeupdate.cpp 

MOC_DIR = moc

DEFINES += GEOCODEUPDATEDLL

symbian: 
{ 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x2002C3A9
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
    
    qtAddLibrary(QtContacts)
	LIBS += -lcntmodel \
	        -lagendainterface \
		    -leuser  \
		    -lestor \
		    -lflogger
   

}  

defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/geocodeupdate.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/geocodeupdate.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock

# End of file	--Don't remove this.

symbian:MMP_RULES += SMPSAFE
