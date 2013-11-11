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

#include(../tsrc.pri)

DEFINES += PBK_UNIT_TEST
DEPENDPATH += .
INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ../inc

#DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc

# Input
    HEADERS += inc\pt_logscntfinder.h 
    HEADERS += inc\pt_logscntfinderqwerty.h 
    SOURCES += src\pt_logscntfinder.cpp   
    SOURCES += src\pt_logscntfinderqwerty.cpp   
    SOURCES += src\main.cpp   

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEfa329b3
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += \
        -lxqservice \
        -lQtContacts \
        -llogscntfinder
    MMP_RULES += SMPSAFE
}
