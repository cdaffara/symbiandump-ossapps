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
# Description: mpcollectiondocumentloader unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpcollectiondocumentloader
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)

HEADERS += inc/unittest_mpcollectiondocumentloader.h \
           ../../inc/mpcollectiondocumentloader.h \
           stub/inc/mpnowplayingwidget.h \
           stub/inc/hgmediawall.h

SOURCES += src/unittest_mpcollectiondocumentloader.cpp \
           ../../src/mpcollectiondocumentloader.cpp \
           stub/src/mpnowplayingwidget.cpp \
           stub/src/hgmediawall.cpp

