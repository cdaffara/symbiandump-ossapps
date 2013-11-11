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
CONFIG += hb

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += /orbit/include # This must be included after the HB stubs
INCLUDEPATH += /epoc32/include
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += /epoc32/include/ecom
INCLUDEPATH += ../../../tsrc/qtestutils/inc

# Input
HEADERS += inc/at_logsengine.h

SOURCES += src/main.cpp
SOURCES += src/at_logsengine.cpp
SOURCES += ../../../tsrc/qtestutils/src/testrunner.cpp

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEfa559b2
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -llogsengine
    MMP_RULES += SMPSAFE
}

BLD_INF_RULES.prj_mmpfiles = "../../../tsrc/logdbmodifier/addeventtest/group/logclienttest.mmp"\
                             "../../../tsrc/logdbmodifier/clearlisttest/group/clearlisttest.mmp"
                             
BLD_INF_RULES.prj_exports += "../../../tsrc/logdbmodifier/addeventtest/data/testlogclients.dat /epoc32/winscw/c/system/testlogclients.dat"
