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

TEMPLATE = app
CONFIG += qtestlib \
					symbian_test \
    hb
TARGET = 

DEPENDPATH += .
INCLUDEPATH += ./stub/inc \
    ../../inc \
    ../../../../inc


    
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

LIBS += -lmpxviewframeworkqt.dll \
        -lxqplugins
        
HEADERS += inc/unittest_mpdetailsviewplugin.h \        
           ../../inc/mpdetailsviewplugin.h \
           stub/inc/mpdetailsview.h            
               
SOURCES += src/unittest_mpdetailsviewplugin.cpp \
		       ../../src/mpdetailsviewplugin.cpp \
		       stub/src/mpdetailsview.cpp
           