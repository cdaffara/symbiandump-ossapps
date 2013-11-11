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
CONFIG  += hb
DEFINES += QT_NO_DEBUG_OUTPUT

DEPENDPATH += .
INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ../inc

DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../../tsrc/qtestutils/inc

# Input
HEADERS += inc/st_logscntfinder.h 
HEADERS += inc/st_logscntfinderqwerty.h 
HEADERS += inc/st_logscntfinderthai.h
            
SOURCES += src/main.cpp   
SOURCES += src/st_logscntfinder.cpp   
SOURCES += src/st_logscntfinderqwerty.cpp
SOURCES += src/st_logscntfinderthai.cpp
SOURCES += ../../../tsrc/qtestutils/src/testresultxmlparser.cpp

symbian: {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lflogger -lqtcontacts -llogscntfinder
    MMP_RULES += SMPSAFE
}
