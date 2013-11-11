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

TARGET = unittest_cxesettingsimp

HEADERS *= unittest_cxesettingsimp.h \
    cxefakesettingsstore.h \
    cxesettings.h \
    cxesettingsimp.h \
    cxutils.h \
    cxenamespace.h \
    cxeerror.h \
    cxescenemodestore.h

SOURCES *= unittest_cxesettingsimp.cpp \
    cxesettingsimp.cpp \
    cxefakesettingsstore.cpp \
    cxescenemodestore.cpp

