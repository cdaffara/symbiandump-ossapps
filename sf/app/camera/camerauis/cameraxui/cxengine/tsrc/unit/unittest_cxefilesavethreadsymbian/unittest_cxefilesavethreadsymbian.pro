# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

# This unit test doesn't want to include system include path of real xqsettingsmanager api
CONFIG *= cxeunit_no_system_paths

include(../unittest.pri)

INCLUDEPATH = . ../utils ../system_include ../../fakeclasses ../../../traces
DEPENDPATH  = . ../utils ../system_include ../../fakeclasses ../../../traces

TARGET = unittest_cxefilesavethreadsymbian

LIBS *= -lsysutil
LIBS *= -lplatformenv
LIBS *= -lefsrv
LIBS *= -lcone

SOURCES *= ./unittest_cxefilesavethreadsymbian.cpp \
           thumbnailmanager_qt.cpp \
           harvesterclient.cpp \
           ../../../src/cxefilesavethread.cpp \
           ../../../src/cxefilesavethreadsymbian.cpp \
           cxefakeimagedataitem.cpp \
           ../../../src/cxeimagedataitemsymbian.cpp \
           ../../../src/cxeharvestercontrolsymbian.cpp \
           ../../../src/cxethumbnailmanagersymbian.cpp \
           ../../../src/cxesysutil.cpp \
           ../../../src/cxestatemachinebase.cpp \
           ../../../src/cxestatemachine.cpp \
           ../../../src/cxestate.cpp \
           cxetestutils.cpp \
           ../../../src/cxeerrormappingsymbian.cpp

HEADERS *= ./unittest_cxefilesavethreadsymbian.h \
           thumbnailmanager_qt.h \
           harvesterclient.h \
           ../../../inc/cxefilesavethread.h \
           ../../../inc/cxefilesavethreadsymbian.h \
           cxefakeimagedataitem.h \
           ../../../inc/cxeimagedataitemsymbian.h \
           ../../../inc/api/cxeimagedataitem.h \
           ../../../inc/api/cxeerror.h \
           ../../../inc/cxeharvestercontrolsymbian.h \
           ../../../inc/cxethumbnailmanagersymbian.h \
           ../../../inc/cxesysutil.h \
           ../../../inc/cxestatemachinebase.h \
           ../../../inc/cxestatemachine.h \
           ../../../inc/cxestate.h \
           cxetestutils.h \
           ../../../inc/cxeerrormappingsymbian.h

