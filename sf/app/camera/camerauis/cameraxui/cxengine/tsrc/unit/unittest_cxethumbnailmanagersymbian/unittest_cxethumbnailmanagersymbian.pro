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

CONFIG *= cxeunit_no_system_paths

include(../unittest.pri)

TARGET = unittest_cxethumbnailmanagersymbian

# Override include paths so that our stub implementation of system classes is used.
DEPENDPATH  = . ../system_include ../utils
INCLUDEPATH = . ../system_include ../utils

HEADERS *= unittest_cxethumbnailmanagersymbian.h \
           ../../../inc/cxethumbnailmanagersymbian.h \
           thumbnailmanager_qt.h \
           ../../../inc/api/cxutils.h

SOURCES *= unittest_cxethumbnailmanagersymbian.cpp \
           thumbnailmanager_qt.cpp \
           ../../../src/cxethumbnailmanagersymbian.cpp
