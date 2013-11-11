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
CONFIG += qtestlib
CONFIG += hb

DEPENDPATH += .
PREPEND_INCLUDEPATH = ../stubs
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += ../../../../tsrc/qtestutils/inc

DEFINES += QT_NO_DEBUG_OUTPUT

# Input
HEADERS += ../stubs/xqsettingskey.h
HEADERS += ../stubs/xqsettingsmanager.h
HEADERS += inc/ut_logsdbconnector.h
HEADERS += inc/ut_logsreader.h
HEADERS += inc/ut_logsreaderstates.h
HEADERS += inc/ut_logseventparser.h
HEADERS += inc/ut_logseventdataparser.h
HEADERS += inc/ut_logsremove.h
HEADERS += inc/ut_logsremovestates.h
HEADERS += inc/ut_logsforegroundwatcher.h
HEADERS += ../../inc/logsdbconnector.h
HEADERS += ../../inc/logsworker.h
HEADERS += ../../inc/logsreader.h
HEADERS += ../../inc/logsreaderstates.h
HEADERS += ../../inc/logsreaderobserver.h
HEADERS += ../../inc/logsremove.h
HEADERS += ../../inc/logsremovestates.h
HEADERS += ../../inc/logsforegroundwatcher.h
HEADERS += ../../../../../recents_plat/logs_engine_api/inc/logsevent.h
HEADERS += ../../../inc/logseventdata.h
HEADERS += ../../../inc/logscommondata.h
HEADERS += ../../../inc/logsconfigurationparams.h

HEADERS += ../stubs/logclient_stubs_helper.h

SOURCES += src/main.cpp
SOURCES += src/ut_logsdbconnector.cpp 
SOURCES += src/ut_logsreader.cpp
SOURCES += src/ut_logsreaderstates.cpp
SOURCES += src/ut_logseventparser.cpp
SOURCES += src/ut_logseventdataparser.cpp
SOURCES += src/ut_logsremove.cpp
SOURCES += src/ut_logsremovestates.cpp
SOURCES += src/ut_logsforegroundwatcher.cpp
SOURCES += ../../src/logsdbconnector.cpp
SOURCES += ../../src/logsworker.cpp
SOURCES += ../../src/logsreader.cpp
SOURCES += ../../src/logsstatebase.cpp
SOURCES += ../../src/logsreaderstates.cpp
SOURCES += ../../src/logseventparser.cpp
SOURCES += ../../src/logseventdataparser.cpp
SOURCES += ../../src/logsremove.cpp
SOURCES += ../../src/logsremovestates.cpp
SOURCES += ../../src/logsforegroundwatcher.cpp
SOURCES += ../../../src/logsevent.cpp
SOURCES += ../../../src/logseventdata.cpp
SOURCES += ../../../src/logscommondata.cpp
SOURCES += ../../../src/logsconfigurationparams.cpp
SOURCES += ../../../src/logsduplicatelookup.cpp

SOURCES += ../../../../tsrc/qtestutils/src/testrunner.cpp
SOURCES += ../stubs/logclient_stubs.cpp
SOURCES += ../stubs/qtcontacts_stubs.cpp
SOURCES += ../stubs/xqsettingsmanager_stub.cpp

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEfaaa9b2
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lecom -lflogger -lws32 -lbafl -lqtcontacts -lefsrv -lapparc -lcone
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES += SMPSAFE
}
