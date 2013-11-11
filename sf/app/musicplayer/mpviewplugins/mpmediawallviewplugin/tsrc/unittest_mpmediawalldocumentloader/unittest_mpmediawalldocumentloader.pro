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
# Description: mpmediawalldocumentloader unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmediawalldocumentloader
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

HEADERS += stub/inc/hgmediawall.h \
           inc/unittest_mpmediawalldocumentloader.h \
           ../../inc/mpmediawalldocumentloader.h

SOURCES += stub/src/hgmediawall.cpp \
           src/unittest_mpmediawalldocumentloader.cpp \
           ../../src/mpmediawalldocumentloader.cpp

