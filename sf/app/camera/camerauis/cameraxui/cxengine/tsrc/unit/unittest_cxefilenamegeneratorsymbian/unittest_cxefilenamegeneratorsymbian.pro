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

include(../unittest.pri)

TARGET = unittest_cxefilenamegeneratorsymbian

LIBS *= -lplatformenv
LIBS *= -lsysutil
LIBS *= -lefsrv
LIBS *= -lcone

SOURCES *= unittest_cxefilenamegeneratorsymbian.cpp \
           cxefakesettings.cpp \
           cxesysutil.cpp \
           cxeerrormappingsymbian.cpp \
           cxefilenamegeneratorsymbian.cpp

HEADERS *= unittest_cxefilenamegeneratorsymbian.h \
           cxefakesettings.h \
           cxesysutil.h \
           cxesettings.h


