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
# Description: Project file for iad wrapper test
#


TEMPLATE = app
TARGET =
DEFINES += BUILD_VIDEOPLAYERAPP_DLL

DEPENDPATH += . \
    inc \
    src

INCLUDEPATH = . \
              stub/inc \
              ../../../inc \
              ../../../../inc \
              $$MW_LAYER_SYSTEMINCLUDE

CONFIG += qtestlib hb qt

LIBS +=

HEADERS += stub/inc/iaupdate.h \
           stub/inc/iaupdateparameters.h \
           stub/inc/iaupdateresult.h \
           stub/inc/featmgr.h \
           inc/testvideoiadupdatewrapper.h \
           ../../inc/videoiadupdatewrapper.h \

SOURCES += \
#           stub/src/iaupdate.cpp \
#           stub/src/iaupdateparameters.cpp \
#           stub/src/iaupdateresult.cpp \
#           stub/src/featmgr.cpp \
           src/testvideoiadupdatewrapper.cpp \
           ../../src/videoiadupdatewrapper.cpp \
