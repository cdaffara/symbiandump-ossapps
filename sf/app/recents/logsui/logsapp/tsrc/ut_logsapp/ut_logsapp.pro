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
CONFIG += hb
RESOURCES += ../../logsapp.qrc

DEPENDPATH += .
PREPEND_INCLUDEPATH = stubs
INCLUDEPATH += .
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += logsengine_stub
INCLUDEPATH += ../../../logsengine/inc
INCLUDEPATH += ../../../logsengine/logssymbianos
INCLUDEPATH += ../../../logsengine/logssymbianos/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../tsrc/qtestutils/inc

DEFINES += QT_NO_DEBUG_OUTPUT

# Input

HEADERS += ./stubs/xqkeycapture.h
HEADERS += ./stubs/xqsettingskey.h
HEADERS += ./stubs/xqsettingsmanager.h
HEADERS += ./stubs/tstasksettings.h
HEADERS += ./stubs/hbapplication.h
HEADERS += inc/ut_logsmainwindow.h
HEADERS += inc/ut_logscomponentrepository.h
HEADERS += inc/ut_logsviewmanager.h
HEADERS += inc/ut_logsbaseview.h
HEADERS += inc/ut_logsrecentcallsview.h
HEADERS += inc/ut_logsdetailsview.h
HEADERS += inc/ut_logsmatchesview.h
HEADERS += inc/ut_logseffecthandler.h
HEADERS += inc/ut_logspageindicator.h
HEADERS += inc/ut_logspageindicatoritem.h
HEADERS += inc/ut_logsservicehandler.h
HEADERS += inc/ut_logsapplication.h
HEADERS += inc/ut_logsappsettings.h
HEADERS += ../../inc/logsmainwindow.h
HEADERS += ../../inc/logsapplication.h
HEADERS += ../../inc/logsappsettings.h
HEADERS += ../../inc/logsviewmanager.h
HEADERS += ../../inc/logscomponentrepository.h
HEADERS += ../../inc/logsrecentcallsview.h
HEADERS += ../../inc/logsbaseview.h
HEADERS += ../../inc/logsdetailsview.h
HEADERS += ../../inc/logsmatchesview.h
HEADERS += ../../inc/logsservicehandler.h
HEADERS += ../../inc/logsservicehandlerold.h
HEADERS += ../../inc/logseffecthandler.h
HEADERS += ../../inc/logspageindicator.h
HEADERS += ../../inc/logspageindicatoritem.h
HEADERS += ../../../logsengine/inc/logsconfigurationparams.h
HEADERS += ../../../logsengine/logssymbianos/inc/logsforegroundwatcher.h
HEADERS += logsengine_stub/logscontact.h
HEADERS += ./stubs/dialpad.h
HEADERS += ./stubs/dialpadkeyhandler.h

SOURCES += src/main.cpp 
SOURCES += src/ut_logsmainwindow.cpp
SOURCES += ../../src/logsmainwindow.cpp
SOURCES += src/ut_logscomponentrepository.cpp
SOURCES += ../../src/logscomponentrepository.cpp
SOURCES += src/ut_logsviewmanager.cpp
SOURCES += ../../src/logsviewmanager.cpp
SOURCES += logsengine_stub/logsabstractmodel.cpp
SOURCES += logsengine_stub/logsmodel.cpp
SOURCES += logsengine_stub/logsdetailsmodel.cpp
SOURCES += logsengine_stub/logscall.cpp
SOURCES += logsengine_stub/logsmessage.cpp
SOURCES += logsengine_stub/logsevent.cpp
SOURCES += logsengine_stub/logseventdata.cpp
SOURCES += logsengine_stub/logscontact.cpp
SOURCES += logsengine_stub/logsmatchesmodel.cpp
SOURCES += ../../../logsengine/src/logsconfigurationparams.cpp
SOURCES += src/ut_logsbaseview.cpp
SOURCES += src/ut_logsrecentcallsview.cpp
SOURCES += src/ut_logsdetailsview.cpp
SOURCES += src/ut_logsmatchesview.cpp
SOURCES += src/ut_logseffecthandler.cpp
SOURCES += src/ut_logsservicehandler.cpp
SOURCES += src/ut_logspageindicator.cpp
SOURCES += src/ut_logspageindicatoritem.cpp
SOURCES += src/ut_logsapplication.cpp
SOURCES += src/ut_logsappsettings.cpp
SOURCES += ../../src/logsbaseview.cpp
SOURCES += ../../src/logsrecentcallsview.cpp
SOURCES += ../../src/logsdetailsview.cpp
SOURCES += ../../src/logsmatchesview.cpp
SOURCES += ../../src/logsservicehandler.cpp
SOURCES += ../../src/logsservicehandlerold.cpp
SOURCES += ../../src/logseffecthandler.cpp
SOURCES += ../../src/logspageindicator.cpp
SOURCES += ../../src/logspageindicatoritem.cpp
SOURCES += ../../src/logsapplication.cpp
SOURCES += ../../src/logsappsettings.cpp
SOURCES += ../../../logsengine/logssymbianos/src/logsforegroundwatcher.cpp
SOURCES += ../../../tsrc/qtestutils/src/testrunner.cpp
SOURCES += ./stubs/hbstubs.cpp
SOURCES += ./stubs/qthighway_stub.cpp
SOURCES += ./stubs/dialpad_stub.cpp
SOURCES += ./stubs/dialpadkeyhandler_stub.cpp
SOURCES += ./stubs/xqsettingsmanager_stub.cpp

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEb768cbc
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lxqservice  -lxqserviceutil -lxqkeycapture -lapparc -lcone
    MMP_RULES += SMPSAFE
}

DOCML += ../../resources/recentCallsView.docml
DOCML += ../../resources/matchesView.docml
DOCML += ../../resources/detailsView.docml
