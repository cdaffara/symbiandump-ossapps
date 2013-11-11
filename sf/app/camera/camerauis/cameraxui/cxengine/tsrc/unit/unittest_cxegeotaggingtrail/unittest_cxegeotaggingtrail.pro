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

# This unit test doesn't want to include system include path of real header api
CONFIG *= cxeunit_no_system_paths

include(../unittest.pri)

INCLUDEPATH *= ../system_include
DEPENDPATH  *= ../system_include

TARGET = unittest_cxegeotaggingtrail


LIBS *= -lecamadvsettings
LIBS *= -lecamsnapshot
LIBS *= -lfbscli
LIBS *= -lbitgdi
LIBS *= -lsysutil
LIBS *= -lecam
LIBS *= -lecampluginsupport
LIBS *= -lplatformenv
LIBS *= -lefsrv
LIBS += -llocationmanager

DEFINES *= CXE_USE_DUMMY_CAMERA

SOURCES *= unittest_cxegeotaggingtrail.cpp \
           rlocationtrail.cpp \
           harvesterclient.cpp \
           cxegeotaggingtrail.cpp \
           cxegeotaggingtrail_symbian_p.cpp \
           cxefakeimagedataitem.cpp \
           cxefakevideocapturecontrol.cpp \
           cxefakestillcapturecontrol.cpp \
           cxefakesettings.cpp \
           cxesysutil.cpp \
           cxestatemachinebase.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxeerrormappingsymbian.cpp \
           cxefakequalitypresets.cpp

HEADERS *= unittest_cxegeotaggingtrail.h \
           rlocationtrail.h \
           harvesterclient.h \
           cxegeotaggingtrail.h \
           cxegeotaggingtrail_symbian_p.h \
           cxefakeimagedataitem.h \
           cxefakevideocapturecontrol.h \
           cxeimagedataitem.h \
           cxeerror.h \
           cxefakestillcapturecontrol.h \
           cxestillcapturecontrol.h \
           cxevideocapturecontrol.h \
           cxesettings.h \
           cxefakequalitypresets.h \
           cxequalitypresets.h \
           cxefakesettings.h \
           cxesysutil.h \
           cxestatemachinebase.h \
           cxestatemachine.h \
           cxestate.h \
           cxeerrormappingsymbian.h

