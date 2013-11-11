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
# Description: mpcollectioncontainers unit test project file.
#

TEMPLATE = app
CONFIG += qtestlib hb symbian_test
TARGET = unittest_mpcollectioncontainers
TARGET.CAPABILITY = CAP_APPLICATION

DEPENDPATH += .
INCLUDEPATH += . \
               stub/inc \
               ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)

LIBS += -lestor.dll \
    	-lmpnowplayingbanner.dll \
        -lganeswidgets.dll

HEADERS += inc/unittest_mpcollectioncontainers.h \
           ../../inc/mpcollectioncontainerfactory.h \
           ../../inc/mpcollectioncontainer.h \
           ../../inc/mpcollectionlistcontainer.h \
           ../../inc/mpcollectioncontainerallsongs.h \
           ../../inc/mpcollectioncontainerartists.h \
           ../../inc/mpcollectioncontaineralbums.h \
           ../../inc/mpcollectioncontainerplaylists.h \
           ../../inc/mpcollectiondocumentloader.h \
           stub/inc/mpcollectiondatamodel.h \
           stub/inc/mpcollectiontbonelistdatamodel.h \
           stub/inc/mpmpxcollectiondata.h \
           stub/inc/mpcollectionview.h \
    	   stub/inc/hgmediawall.h \
	   stub/inc/mpnowplayingwidget.h\
	   stub/inc/mpplaybackdata.h \
	   stub/inc/mpengine.h \
	   stub/inc/mpenginefactory.h
	  
SOURCES += src/unittest_mpcollectioncontainers.cpp \
           ../../src/mpcollectioncontainerfactory.cpp \
           ../../src/mpcollectioncontainer.cpp \
           ../../src/mpcollectionlistcontainer.cpp \
           ../../src/mpcollectioncontainerallsongs.cpp \
           ../../src/mpcollectioncontainerartists.cpp \
           ../../src/mpcollectioncontaineralbums.cpp \
           ../../src/mpcollectioncontainerplaylists.cpp \
           ../../src/mpcollectiondocumentloader.cpp \
           stub/src/mpcollectiondatamodel.cpp \
           stub/src/mpcollectiontbonelistdatamodel.cpp \
           stub/src/mpmpxcollectiondata.cpp \
           stub/src/mpcollectionview.cpp \
	   stub/src/hgmediawall.cpp \
	   stub/src/mpnowplayingwidget.cpp \
	   stub/src/mpplaybackdata.cpp \
	   stub/src/mpengine.cpp \
	   stub/src/mpenginefactory.cpp
	  

RESOURCES += ../../resources/mpcollectionviewresources.qrc

