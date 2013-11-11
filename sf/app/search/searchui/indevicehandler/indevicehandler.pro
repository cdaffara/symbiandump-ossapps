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
# Description:  SEARCH state provider project file
#

TEMPLATE = lib
TARGET = searchindevicehandler

   
   LIBS += -lcpixsearch
   LIBS += -lhbcore
   
DEFINES += BUILD_INDEVICEHANDLER 

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
               ../inc 

symbian {
    TARGET.UID2 = 0x20004C45
    TARGET.UID3 = 0x2002C37B
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        
    defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" 	\
	"DEFFILE  ../eabi/" 				\
    "$${LITERAL_HASH}else" 				\
    "DEFFILE  ../bwins/" 				\
    "$${LITERAL_HASH}endif"
	
	MMP_RULES += defBlock

}


include(indevicehandler.pri)

