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
# Description: Project file for Music Player Data.
#

TEMPLATE = lib
CONFIG += hb
TARGET = mpdata
symbian: { 
    TARGET.UID3 = 0x10207C95
    MMP_RULES += "DEFFILE mpdata.def" SMPSAFE
    defFilePath = .
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
}
DEFINES += BUILD_MPDATA_LIB

INCLUDEPATH += . \
    	       inc \
    	       ../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets)

LIBS += -lestor \
        -lmpxcommon \
        -lthumbnailmanagerqt

# Input
HEADERS += ../inc/mpmpxcollectiondata.h \
           inc/mpmpxcollectiondata_p.h \
           ../inc/mpcollectiondatamodel.h \
           ../inc/mpcollectiontbonelistdatamodel.h \
    	   inc/mpcollectionalbumartmanager.h \
    	   ../inc/mpplaybackdata.h \
    	   ../inc/mpsongdata.h \
    	   inc/mpsongdata_p.h

SOURCES += src/mpmpxcollectiondata.cpp \
           src/mpmpxcollectiondata_p.cpp \
           src/mpcollectiondatamodel.cpp \
           src/mpcollectiontbonelistdatamodel.cpp \
    	   src/mpcollectionalbumartmanager.cpp \
           src/mpplaybackdata.cpp \
           src/mpsongdata.cpp \
           src/mpsongdata_p.cpp

