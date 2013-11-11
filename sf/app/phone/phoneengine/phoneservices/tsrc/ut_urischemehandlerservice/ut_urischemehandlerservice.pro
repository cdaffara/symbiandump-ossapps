#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
TARGET.UID3 = 0xE31F4EDA
DEPENDPATH += .
INCLUDEPATH += . ../shared
CONFIG += hb qtestlib
DEFINES += QT_NO_DEBUG_STREAM XQ_BUILD_XQSERVICEUTIL_LIB XQ_BUILD_XQSERVICE_LIB

QT -= gui
DEFINES -= QT_GUI_LIB

symbian {
    TARGET.CAPABILITY = ALL -TCB
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += ../../../../inc
    
    # TEST CODE
    HEADERS += ../../inc/urischemehandlerservice.h
    SOURCES += ../../src/urischemehandlerservice.cpp
    SOURCES += unit_tests.cpp
    
    #MOCKED DEPENDENCIES
    HEADERS += /epoc32/include/mw/xqserviceprovider.h
    HEADERS += ../../inc/urischemeparser.h
    HEADERS += ../../inc/ctiurischemeparser.h
    HEADERS += ../../inc/telurischemeparser.h
    SOURCES += ../shared/mock_hbdevicemessagebox.cpp
    SOURCES += ../shared/mock_urischemeparser.cpp
    SOURCES += ../shared/mock_telurischemeparser.cpp
    SOURCES += ../shared/mock_ctiurischemeparser.cpp
    SOURCES += ../shared/mock_xqrequestinfo.cpp
    SOURCES += ../shared/mock_mpecallsettersif.cpp
    SOURCES += ../shared/mock_mpecallcontrolif.cpp
    SOURCES += ../shared/mock_xqserviceprovider.cpp
    
    LIBS += -lmocklib -lsymbianmock# -lxqservice
}


symbian:MMP_RULES += SMPSAFE
