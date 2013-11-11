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

TEMPLATE = lib
CONFIG += hb qt dll
TARGET = 
DEFINES += BUILD_VIDEOCOLLECTION_DLL

symbian: { 
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x200211FD
    BLD_INF_RULES.prj_exports += "rom/videocollectionwrapper.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videocollectionwrapper.iby)"
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    defBlock = \      
        "$${LITERAL_HASH}if defined(EABI)" \
        "DEFFILE ../eabi/videocollectionwrapper.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE ../bwins/videocollectionwrapper.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defBlock SMPSAFE
    VERSION = 10.0
}

DEPENDPATH += . inc src

INCLUDEPATH += .

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

INCLUDEPATH += $$OS_LAYER_STDCPP_SYSTEMINCLUDE

INCLUDEPATH += ../inc \
               /epoc32/include/mw/hb/hbtools

# Input
HEADERS += inc/videodatasignalreceiver.h \
           inc/videocollectionclient.h \
           inc/videocollectionlistener.h \
           inc/videocollectionwrapper.h \
           inc/videocollectionwrapper_p.h \
           inc/videolistdatamodel.h \
           inc/videolistdatamodel_p.h \
           inc/videothumbnaildata.h \
           inc/videothumbnaildata_p.h \
           inc/videothumbnailfetcher.h \
           inc/videocollectionutils.h \
           inc/videoproxymodelgeneric.h \
           inc/videoproxymodelallvideos.h \
           inc/videoproxymodelcollections.h \
           inc/videoproxymodelcontent.h \
           inc/videodatacontainer.h \
           inc/videodeleteworker.h \
           ../inc/videocollectionexport.h
           
SOURCES += src/videocollectionclient.cpp \
           src/videocollectionlistener.cpp \
           src/videocollectionwrapper.cpp \
           src/videocollectionwrapper_p.cpp \
           src/videolistdatamodel.cpp \
           src/videolistdatamodel_p.cpp \
           src/videothumbnaildata.cpp \
           src/videothumbnaildata_p.cpp \
           src/videothumbnailfetcher.cpp \
           src/videocollectionutils.cpp \
           src/videoproxymodelgeneric.cpp \
           src/videoproxymodelallvideos.cpp \
           src/videoproxymodelcollections.cpp \
           src/videoproxymodelcontent.cpp \
           src/videodeleteworker.cpp \
           src/videodatacontainer.cpp
          
LIBS += -lmpxcommon.dll \
        -lmpxcollectionutility.dll \
        -lthumbnailmanagerqt.dll \
        -lestor.dll \
        -lcentralrepository.dll \
        -lflogger.dll
