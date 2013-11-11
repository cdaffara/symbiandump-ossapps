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

TEMPLATE = app
TARGET = t_radiohswidget

DEFINES -= XQSETTINGSMANAGER_LIBRARY
DEFINES += XQSETTINGSMANAGER_NO_LIBRARY
DEFINES -= XQSERVICE_EXPORT
DEFINES += XQ_BUILD_XQSERVICE_LIB
DEFINES -= XQSERVICEUTIL_EXPORT
DEFINES += XQ_BUILD_XQSERVICEUTIL_LIB

symbian: {
    TARGET.CAPABILITY = All -TCB -DRM
    CONFIG -= hb
    MMP_RULES += SMPSAFE
}

INCLUDEPATH += ../stub
INCLUDEPATH += ../../common

QT += core
QT += testlib

CONFIG += qtestlib

HEADERS += inc/t_radiohswidget.h
HEADERS += inc/radiohswidget.h
HEADERS += ../stub/xqsettingsmanager.h
HEADERS += ../stub/xqsettingskey.h
HEADERS += ../stub/xqapplicationmanager.h
HEADERS += ../stub/xqaiwrequest.h
HEADERS += ../stub/xqrequestinfo.h
HEADERS += ../stub/xqaiwinterfacedescriptor.h
HEADERS += ../inc/radiohswidgetprofilereader.h
HEADERS += ../inc/radiohswidgetradioserviceclient.h

SOURCES += src/t_radiohswidget.cpp
SOURCES += src/radiohswidget.cpp
SOURCES += ../stub/xqsettingsmanager.cpp
SOURCES += ../stub/xqsettingskey.cpp
SOURCES += ../stub/xqapplicationmanager.cpp
SOURCES += ../stub/xqaiwrequest.cpp
SOURCES += ../stub/xqrequestinfo.cpp
SOURCES += ../stub/xqaiwinterfacedescriptor.cpp
SOURCES += ../src/radiohswidgetprofilereader.cpp
SOURCES += ../src/radiohswidgetradioserviceclient.cpp
SOURCES += ../../common/radiologger.cpp

include(buildflags.pri)
