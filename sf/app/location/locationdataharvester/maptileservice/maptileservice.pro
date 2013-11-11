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
TARGET = maptileservice   

CONFIG += dll
CONFIG += hb
CONFIG += mobility
MOBILITY = publishsubscribe
QT           += sql

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../mylocationlogger/inc


MOC_DIR = moc

DEFINES += MAPTILESERVICEDLL

INTERNAL_PUBLIC_HEADERS +=  inc/maptileservice.h        

# Input
HEADERS +=  $$INTERNAL_PUBLIC_HEADERS \
           ./inc/maptiledblookuptable.h  
           
               
    
SOURCES += ./src/maptileservice.cpp ./src/maptiledblookuptable.cpp

symbian: 
{

    :BLD_INF_RULES.prj_exports += "conf/maptileservice.confml APP_LAYER_CONFML(maptileservice.confml)"
    :BLD_INF_RULES.prj_exports += "conf/maptileservice_2002E6E8.crml APP_LAYER_CRML(maptileservice_2002E6E8.crml)"
    :BLD_INF_RULES.prj_exports += "conf/2002E6E8.txt /epoc32/winscw/c/private/10202be9/2002E6E8.txt"
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x2002E6E8
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    
   LIBS += -ledbms  \
           -lbafl \
           -lcentralrepository \
           -leuser \
           -lflogger \
           -lefsrv
   
        myCrml.sources = ./conf/maptilecontactpublisher.qcrml
        myCrml.path = c:/resource/qt/crml
        DEPLOYMENT += myCrml
        
        myCrml1.sources = ./conf/maptilecalpublisher.qcrml
        myCrml1.path = c:/resource/qt/crml
        DEPLOYMENT += myCrml1

}  

deploy.path = /
headers.sources = $$INTERNAL_PUBLIC_HEADERS
headers.path = epoc32/include/app
DEPLOYMENT += exportheaders

# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"

defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/maptileservice.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/maptileservice.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock



symbian:MMP_RULES += SMPSAFE

