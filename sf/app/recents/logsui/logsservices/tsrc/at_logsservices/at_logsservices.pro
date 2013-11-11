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
#

TEMPLATE = app
TARGET = 

QT += testlib xml
CONFIG  += qtestlib

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ./inc
INCLUDEPATH += /orbit/include # This must be included after the HB stubs
INCLUDEPATH += /epoc32/include
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += /epoc32/include/ecom
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../../tsrc/qtestutils/inc

# Input
HEADERS += inc/at_logsservices.h

SOURCES += src/main.cpp
SOURCES += src/at_logsservices.cpp
SOURCES += ../../../tsrc/qtestutils/src/testresultxmlparser.cpp

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEf1af9b2
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lecom -lflogger -lws32 -lbafl -llogsservices
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES += SMPSAFE
}
