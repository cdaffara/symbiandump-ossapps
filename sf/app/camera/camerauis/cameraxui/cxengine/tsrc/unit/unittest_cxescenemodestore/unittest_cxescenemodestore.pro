# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Unit test project for CxeSceneModeStore class

include(../unittest.pri)

TARGET = unittest_cxescenemodestore

SOURCES *= unittest_cxescenemodestore.cpp \
           cxescenemodestore.cpp

HEADERS *= \
    unittest_cxescenemodestore.h \
    cxescenemodestore.h
