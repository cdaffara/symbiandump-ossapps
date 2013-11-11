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

TMP_DIR_NAME = enginewrapper
include(../buildflags.pri)

TEMPLATE    = lib
TARGET      = fmradioenginewrapper
CONFIG      += dll
DEFINES     += BUILD_WRAPPER_DLL

symbian:TARGET.UID3 = 0x2002EAD8
    
USE_DUMMY_RADIO_DATA:QT += xml

INCLUDEPATH += inc
INCLUDEPATH += ../../common

# Common headers
HEADERS     += radiowrapperexport.h
HEADERS     += radio_global.h
HEADERS     += radiologger.h
HEADERS     += radioenginewrapper.h
HEADERS     += radiosettingsif.h
HEADERS     += radiosettings.h
HEADERS     += radiostationhandlerif.h
HEADERS     += radioenginewrapperobserver.h

INTERNAL_HEADERS += radiosettings_p.h

#USE_DUMMY_RADIO_DATA:HEADERS += t_radiodataparser.h

# Common sources
SOURCES     += radiosettings.cpp
SOURCES     += radiologger.cpp
SOURCES     += radioenginewrapperobserver.cpp

# Symbian specific stuff
symbian: {
    INCLUDEPATH += ../../radioengine/utils/api
    INCLUDEPATH += ../../radioengine/settings/api
    INCLUDEPATH += ../../radioengine/engine/api

    LIBS *= -lfmradioengineutils
    LIBS *= -lfmradioenginesettings
    LIBS *= -lfmradioengine

    HEADERS += radioenginehandler.h
    HEADERS += mradioenginehandlerobserver.h
    HEADERS += radiocontroleventlistener.h
    HEADERS += radiordslistener.h

    INTERNAL_HEADERS += radioenginewrapper_p.h

    SOURCES += radiosettings_p.cpp
    SOURCES += radioenginewrapper.cpp
    SOURCES += radioenginewrapper_p.cpp
    SOURCES += radioenginehandler.cpp
    SOURCES += radiocontroleventlistener.cpp
    SOURCES += radiordslistener.cpp
}

#USE_DUMMY_RADIO_DATA:SOURCES += t_radiodataparser.cpp

# Win32 specific stuff
win32: {
    INCLUDEPATH += inc/win32

    HEADERS += radioenginewrapper_win32_p.h
    HEADERS += $$INTERNAL_HEADERS

    SOURCES += radiosettings_win32_p.cpp
    SOURCES += radioenginewrapper_win32.cpp
    SOURCES += radioenginewrapper_win32_p.cpp
}

DEPENDPATH  += $$INCLUDEPATH src
