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
TARGET = t_phoneringingtoneplayer 
CONFIG += hb 

symbian {

    INCLUDEPATH += /epoc32/include/platform/mw \
                   /epoc32/include/platform \
                   ../../../phoneuiutils/inc \
                   ../../../phoneuiview2/src \
                   ../../../phoneuiqtviewadapter/inc \
                   ../../inc \
                   ../../../phonemediatorcenter/inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    LIBS += -lphoneringingtoneplayer \
            -lphoneuiutils

    TARGET.CAPABILITY = ALL -TCB
    DEFINES += FT_SYMBIAN_INTEGRATION
}

HEADERS   += t_phoneringingtoneplayer.h
SOURCES   += t_phoneringingtoneplayer_reg.rss \
             main.cpp \
             t_phoneringingtoneplayer.cpp
             
RESOURCES +=

symbian:TARGET.UID3 = 0xE87E453C


symbian:MMP_RULES += SMPSAFE
