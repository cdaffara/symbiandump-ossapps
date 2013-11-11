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
#



CONFIG += debug
TEMPLATE = lib
TARGET = ipssosplugin
QT += core

DEFINES += BUILD_DLL

INCLUDEPATH += inc \
../inc \
../../inc


HEADERS   += inc/ipsplgbaseoperation.h \
inc/ipsplgheaders.h \
inc/ipsplgmrulist.h \
inc/ipsplgpanic.h \
inc/ipsplgdeletelocal.h \
inc/ipsplgdeleteoperation.h \
inc/ipsplgdisconnectop.h \
inc/ipsplgeventtimer.h \
inc/ipsplgimap4connectop.h \
inc/ipsplgimap4fetchattachmentop.h \
inc/ipsplgimap4plugin.h \
inc/ipsplgimap4populateop.h \
inc/ipsplgimap4moveremoteop.h \
inc/ipsplgmrulist.h \
inc/ipsplgmsgiterator.h \
inc/ipsplgmsgkey.h \
inc/ipsplgmsgmapper.h \
inc/ipsplgmsgswap.h \
inc/ipsplgonlineoperation.h \
inc/ipsplgcreatemessageoperation.h \
inc/ipsplgcreateforwardmessageoperation.h \
inc/ipsplgcreatereplymessageoperation.h \
inc/ipsplgnewchildpartfromfileoperation.h \
inc/ipsplgremovechildpartoperation.h \
# <qmail> inc/ipsplgoperationwait.h \ removed
inc/ipsplgpop3connectop.h \
inc/ipsplgpop3fetchoperation.h \
inc/ipsplgpop3plugin.h \
inc/ipsplgsearch.h \
inc/ipsplgsearchop.h \
inc/ipsplgsettingsobserver.h \
inc/ipsplgsingleopwatcher.h \
inc/ipsplgsmtpoperation.h \
inc/ipsplgsmtpservice.h \
inc/ipsplgsosbaseplugin.h \
inc/ipsplgtimeroperation.h \
inc/ipsplgeventhandler.h \
inc/ipsplgpropertywatcher.h \
inc/ipsplgtextsearcher.h \
inc/ipsplgconnectandrefreshfolderlist.h \
inc/ipsplgsyncstatehandler.h \
inc/ipsplgsosbaseplugin.hrh \
inc/ipsplgmailstoreroperation.h \
inc/mipsplgmailstorerobserver.h \
inc/ipsplgmessagepartstoreroperation.h \
inc/ipsstateextension.h \
inc/ipssosextendedsettingsmanager.h

    
SOURCES   += src/ipsplgbaseoperation.cpp \
src/ipsplgdeletelocal.cpp \
src/ipsplgdeleteoperation.cpp \
src/ipsplgdisconnectop.cpp \
src/ipsplgeventtimer.cpp \
src/ipsplgimap4connectop.cpp \
src/ipsplgimap4fetchattachmentop.cpp \
src/ipsplgimap4plugin.cpp \
src/ipsplgimap4populateop.cpp \
src/ipsplgimap4moveremoteop.cpp \
src/ipsplgimplementationtable.cpp \
src/ipsplgmrulist.cpp \
src/ipsplgmsgiterator.cpp \
src/ipsplgmsgkey.cpp \
src/ipsplgmsgmapper.cpp \
src/ipsplgmsgswap.cpp \
src/ipsplgonlineoperation.cpp \
src/ipsplgcreatemessageoperation.cpp \
src/ipsplgcreateforwardmessageoperation.cpp \
src/ipsplgcreatereplymessageoperation.cpp \
src/ipsplgnewchildpartfromfileoperation.cpp \
src/ipsplgremovechildpartoperation.cpp \
# <qmail> removed src/ipsplgoperationwait.cpp \
src/ipsplgpop3connectop.cpp \
src/ipsplgpop3fetchoperation.cpp \
src/ipsplgpop3plugin.cpp \
src/ipsplgsearch.cpp \
src/ipsplgsearchop.cpp \
src/ipsplgsettingsobserver.cpp \
src/ipsplgsingleopwatcher.cpp \
src/ipsplgsmtpoperation.cpp \
src/ipsplgsmtpservice.cpp \
src/ipsplgsosbaseplugin.cpp \
src/ipsplgtimeroperation.cpp \
src/ipsplgeventhandler.cpp \
src/ipsplgpropertywatcher.cpp \
src/ipsplgtextsearcher.cpp \
src/ipsplgconnectandrefreshfolderlist.cpp \
src/ipsplgsyncstatehandler.cpp \
inc/ipsplgsearch.inl \
inc/ipsplgmsgmapper.inl \
src/ipsplgmailstoreroperation.cpp \
src/ipsplgmessagepartstoreroperation.cpp \
src/ipsstateextension.cpp \
src/ipssosextendedsettingsmanager.cpp

RESOURCES +=

symbian*: { 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += /epoc32/include/ecom

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x10009D8D
    TARGET.UID3 = 0x2000E53D
    
    ecomResourceBlock = \
      "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
      "SOURCEPATH       data" \
      "START RESOURCE   2000E53D.rss" \
      "TARGET           ipssosplugin.rsc" \
      "TARGETPATH       ECOM_RESOURCE_DIR" \
      "END"

     

    
    defBlock = \      
    "$${LITERAL_HASH}if defined(MARM)" \
    "DEFFILE  EABI/IPSSOSPLUGIN.DEF" \
    "$${LITERAL_HASH}else" \
    "DEFFILE  BWINS/IPSSOSPLUGIN.DEF" \
    "$${LITERAL_HASH}endif"
    
    MMP_RULES += ecomResourceBlock \
    defBlock
    
    MMP_RULES += "TARGETTYPE plugin"  
    
    LIBS += -lestor \
    -lefsrv \
    -lecom \
    -leuser \
    -lmsgs \
    -letext \
    -limcm \
    -lfsmailframework \
    -lfsfwcommonlib \
    -lbafl \
    -lcentralrepository \
    -lCommonEngine \
    -lcharconv \
    -lConnMon \
    -lflogger \
    -lfeatmgr \
    -lmessagestoreclient \
    -lxqsettingsmanager \
    -lAlwaysOnlineManagerClient

    BLD_INF_RULES.prj_exports += \
      "inc/ipsplgsmtpoperation.h |../inc/ipsplgsmtpoperation.h" \
      "inc/ipsplgsosbaseplugin.h |../inc/ipsplgsosbaseplugin.h" \
      "inc/ipsplgsosbaseplugin.hrh |../inc/ipsplgsosbaseplugin.hrh" \
      "inc/ipsplgimap4populateop.h |../inc/ipsplgimap4populateop.h" \
      "inc/ipsplgonlineoperation.h |../inc/ipsplgonlineoperation.h" \
      "inc/ipsplgbaseoperation.h |../inc/ipsplgbaseoperation.h" \
      "inc/ipsplgconnectandrefreshfolderlist.h |../inc/ipsplgconnectandrefreshfolderlist.h" \
      "inc/ipsplgpropertywatcher.h |../inc/ipsplgpropertywatcher.h"
      
    MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"

    MMP_RULES += SMPSAFE
}

win32 {
   DESTDIR = ../../bin
}



