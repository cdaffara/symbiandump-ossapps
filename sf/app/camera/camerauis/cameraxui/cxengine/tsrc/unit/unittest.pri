#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

!cxeunit_no_system_paths {
    include(../../../camerax.pri)
} else {
    message( "CameraX common system paths NOT included!" )
}

# Common setup for unit tests

TEMPLATE = app

CONFIG += console symbian_test
QT += testlib

DEPENDPATH += . ../../fakeclasses ../../../src ../../../src/dummyengine ../../../inc/api ../../../inc ../utils
INCLUDEPATH += . ../../../inc ../../../inc/api ../../fakeclasses ../../../traces ../../../src/dummyengine ../utils

symbian {
    TARGET.CAPABILITY = ALL -TCB -DRM
    
    DEFINES += CAMERAX_ENGINE_LIBRARY
}


# Common utility classes

SOURCES += cxetestutils.cpp
HEADERS += cxetestutils.h

