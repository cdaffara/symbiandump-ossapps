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
# Description:  SEARCH online handler project file
#

TEMPLATE = lib
TARGET = searchonlinehandler   

CONFIG += hb    
CONFIG += console

LIBS += -lhbcore
LIBS += -lxqsettingsmanager
LIBS += -lisproviderdbmanager
LIBS += -lisengine
   
QT -= webkit
QT += xml
   
DEFINES += SEARCHONLINEHANDLER_LIB 

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc \
               ../inc

symbian {
    TARGET.UID2 = 0x20004C45
    TARGET.UID3 = 0x2002C37C
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
include(onlinehandler.pri)