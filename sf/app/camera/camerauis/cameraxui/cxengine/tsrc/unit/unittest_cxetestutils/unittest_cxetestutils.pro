# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

TARGET = unittest_cxetestutils

SOURCES *= unittest_cxetestutils.cpp \
           cxedummystatemachine.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxestatemachinebase.cpp \
           cxeerrormappingsymbian.cpp

HEADERS *= unittest_cxetestutils.h \
           cxedummystatemachine.h \
           cxestatemachine.h \
           cxestate.h \
           cxestatemachinebase.h \
           cxeerrormappingsymbian.h 



