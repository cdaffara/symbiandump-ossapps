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

include(../unittest.pri)

TARGET = unittest_cxeimagedataqueuesymbian

LIBS *= -lsysutil
LIBS *= -lplatformenv
LIBS *= -lefsrv
LIBS *= -lcone

SOURCES *= unittest_cxeimagedataqueuesymbian.cpp \
           cxeimagedataqueuesymbian.cpp \
           cxeimagedataqueueunit.cpp \
           cxeimagedataitemsymbian.cpp \
           cxefakeimagedataitem.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxestatemachinebase.cpp \
           cxeerrormappingsymbian.cpp \
           cxesysutil.cpp

HEADERS *= unittest_cxeimagedataqueuesymbian.h \
           cxeimagedataqueue.h \
           cxeimagedataqueuesymbian.h \
           cxeimagedataqueueunit.h \
           cxeimagedataitem.h \
           cxeimagedataitemsymbian.h \
           cxefakeimagedataitem.h \
           cxestate.h \
           cxestatemachine.h \
           cxestatemachinebase.h \
           cxeerrormappingsymbian.h \
           cxutils.h \
           cxesysutil.h
