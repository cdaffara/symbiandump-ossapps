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
# Description: FmBkupEngineWrapper project - source files
#

INCLUDEPATH += ./inc
INCLUDEPATH += ../common
win32:INCLUDEPATH += ./private/win32
symbian { 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += /epoc32/include/connect
    INCLUDEPATH += ./private/symbian
    INCLUDEPATH += ../fmbkupengine/inc
}

HEADERS += inc/fmbkupengine.h \
           inc/fmbkupcommon.h \
           inc/fmbackupsettings.h \
           inc/fmrestoresettings.h \


SOURCES += src/fmbkupengine.cpp \
           src/fmbackupsettings.cpp \
           src/fmrestoresettings.cpp \

win32 { 
    HEADERS += private/win32/fmbkupengine_p.h
    SOURCES += private/win32/fmbkupengine_p.cpp
}

symbian { 
    HEADERS += inc/FileManagerPrivateCRKeys.h \
               private/symbian/fmbkupengine_p.h
    SOURCES += private/symbian/fmbkupengine_p.cpp
}
