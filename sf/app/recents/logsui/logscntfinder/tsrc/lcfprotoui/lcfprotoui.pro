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
TARGET = lcfprotoui
CONFIG += hb
RESOURCES += lcfprotoui.qrc

INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
HEADERS += inc/lcfmainwindow.h
HEADERS += inc/lcfsearchresultsview.h
HEADERS += inc/lcfmodel.h

SOURCES += src/main.cpp 
SOURCES += src/lcfmainwindow.cpp
SOURCES += src/lcfsearchresultsview.cpp
SOURCES += src/lcfmodel.cpp


symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x101F4CD6
    TARGET.CAPABILITY = CAP_APPLICATION
    LIBS += -llogscntfinder
    MMP_RULES += SMPSAFE
}

