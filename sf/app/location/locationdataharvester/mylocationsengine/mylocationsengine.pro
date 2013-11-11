//====
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

TEMPLATE=app
TARGET=mylocationsengine
CONFIG += Qt mobility
MOBILITY = publishsubscribe
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../geocodeupdate/inc
INCLUDEPATH += ../mylocationlogger/inc
INCLUDEPATH += ../mylocationsdatabasemanager/inc
INCLUDEPATH += ../maptileservice/inc
INCLUDEPATH += /epoc32/include/app


symbian: { 

    TARGET.UID3 = 0x2002680A 
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x8000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        16943040
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    LIBS += -llbs \
        -leposlandmarks \
        -leposlmsearchlib \
        -leuser \
        -leposlmdbmanlib \
        -lcntmodel \
        -lefsrv \
        -ledbms \
        -lbafl \
        -lflogger \
        -lQtContacts \
        -lgeocodeupdate \
        -lecom \
        -lcentralrepository \
	-lcalinterimapi \
	-llocationdatalookupdb \
	-lmylocationsdatabasemanager \
	-lmaptileservice
	
	
	myCrml.sources = ./conf/maptilestatuspublisher.qcrml
        myCrml.path = c:/resource/qt/crml
        DEPLOYMENT += myCrml

}

SOURCES += src/appmain.cpp \
    src/mylocationsengine.cpp \
    src/maptileinterface.cpp \
    src/calendernotification.cpp \
    src/mylocationgeotagtimerao.cpp \
    src/contactsubscriber.cpp \
    src/calendarsubscriber.cpp

HEADERS += inc/appmain.h \
    inc/mylocationsengine.h \
    inc/maptileinterface.h \
    inc/calendernotification.h \
    inc/mylocationgeotagtimerao.h \
    inc/contactsubscriber.h \
    inc/calendarsubscriber.h






symbian:MMP_RULES += SMPSAFE
