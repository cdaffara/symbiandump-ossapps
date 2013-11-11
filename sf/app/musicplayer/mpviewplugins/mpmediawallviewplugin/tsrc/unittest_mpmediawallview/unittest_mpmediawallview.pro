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
# Description: 
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpmediawallview
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += . 
INCLUDEPATH += ./stub/inc \
	       ../../inc \
	       ../../../../inc
	       

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)

LIBS += -lmpxviewframeworkqt.dll \
        -lmpsettingsmanager.dll


# Input
HEADERS += inc/unittest_mpmediawallview.h \
           ../../inc/mpmediawallview.h \
           ../../../../inc/mpviewbase.h \
           stub/inc/hgmediawall.h \
           stub/inc/mpengine.h \
           stub/inc/mpenginefactory.h \
           stub/inc/mpmpxcollectiondata.h \
           stub/inc/mpcollectiondatamodel.h \
           stub/inc/mpplaybackdata.h \
           stub/inc/mpalbumcoverwidget.h \
           stub/inc/mpmediawalldocumentloader.h \
           stub/inc/mptracklistwidget.h \
           stub/inc/mpcollectiontbonelistdatamodel.h \
           stub/inc/hbmessagebox.h

SOURCES += src/unittest_mpmediawallview.cpp \
           stub/src/hgmediawall.cpp \
           stub/src/mpengine.cpp \
           stub/src/mpenginefactory.cpp \
           stub/src/mpmpxcollectiondata.cpp \
           stub/src/mpcollectiondatamodel.cpp \
           stub/src/mpplaybackdata.cpp \
           stub/src/mpalbumcoverwidget.cpp \
           stub/src/mpmediawalldocumentloader.cpp \
           stub/src/mptracklistwidget.cpp \
           stub/src/mpcollectiontbonelistdatamodel.cpp


RESOURCES += ../../resources/mpmediawallviewresources.qrc
