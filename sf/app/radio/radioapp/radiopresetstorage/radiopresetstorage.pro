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

TMP_DIR_NAME = presetstorage
include(../buildflags.pri)

TEMPLATE    = lib
TARGET      = fmradiopresetstorage
CONFIG      += dll
DEFINES     += BUILD_PRESET_STORAGE_DLL

symbian:TARGET.UID3 = 0x2002EAD9

INCLUDEPATH += inc
INCLUDEPATH += ../../common

symbian:LIBS += -lpresetutility

DEPENDPATH += $$INCLUDEPATH src

# Input
HEADERS     += radiopresetstorage.h
HEADERS     += radiostationif.h
HEADERS     += radiopresetstorageexport.h

# Symbian specific stuff
symbian: {
    INTERNAL_HEADERS += radiopresetstorage_p.h
    SOURCES += radiopresetstorage.cpp
}

# Win32 specific stuff
win32: {
    HEADERS += radiopresetstorage_win32_p.h
    SOURCES += radiopresetstorage_win32.cpp
}
