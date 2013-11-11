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
TARGET = ipssosaoplugin
QT += core

DEFINES += BUILD_DLL

INCLUDEPATH += inc \
../inc \
../../inc


HEADERS   += \
inc/IpsSosAOBaseAgent.h \
inc/IpsSosAOEMNResolver.h \
inc/IpsSosAOImapAgent.h \
inc/IpsSosAOImapPopLogic.h \
inc/IpsSosAOMboxLogic.h \
inc/IpsSosAOPlugin.hrh \
inc/IpsSosAOPluginEComInterface.h \
inc/IpsSosAOPluginTimer.h \
inc/IpsSosAOPopAgent.h \
inc/IpsSosAOSchedulerUtils.h \
inc/IpsSosAOSmtpAgent.h \
inc/IpsSosAOSmtpSendWatcher.h \
inc/IpsSosAOSettingsHandler.h \
inc/IpsSosAoExtendedSettingsManager.h


    
SOURCES   += \
src/IpsPlgAOPluginImplementationTable.cpp \
src/IpsSosAOBaseAgent.cpp \
src/IpsSosAOEMNResolver.cpp \
src/IpsSosAOImapAgent.cpp \
src/IpsSosAOImapPopLogic.cpp \
src/IpsSosAOMboxLogic.cpp \
src/IpsSosAOPluginEComInterface.cpp \
src/IpsSosAOPluginTimer.cpp \
src/IpsSosAOPopAgent.cpp \
src/IpsSosAOSchedulerUtils.cpp \
src/IpsSosAOSmtpAgent.cpp \
src/IpsSosAOSmtpSendWatcher.cpp \
src/IpsSosAOSettingsHandler.cpp \
src/IpsSosAoExtendedSettingsManager.cpp


RESOURCES +=

symbian*: { 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += /epoc32/include/ecom

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x10009D8D
    TARGET.UID3 = 0x200100BA
    
    ecomResourceBlock = \
      "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
      "SOURCEPATH       data" \
      "START RESOURCE   200100BA.rss" \
      "TARGET           ipssosaoplugin.rsc" \
      "TARGETPATH       ECOM_RESOURCE_DIR" \
      "END"

     

    
    defBlock = \      
    "$${LITERAL_HASH}if defined(MARM)" \
    "DEFFILE  EABI/IPSSOSAOPLUGIN.DEF" \
    "$${LITERAL_HASH}else" \
    "DEFFILE  BWINS/IPSSOSAOPLUGIN.DEF" \
    "$${LITERAL_HASH}endif"
    
    MMP_RULES += ecomResourceBlock \
    defBlock
    
    MMP_RULES += "TARGETTYPE plugin"
    MMP_RULES += SMPSAFE
    
    LIBS += \
    -lecom \
    -leuser \
    -lmsgs \
    -letext \
    -limcm \
    -leikcore \
    -lipssosplugin \
    -lxqsettingsmanager \
    -lcmmanager


      
    MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"
      
}

win32 {
   DESTDIR = ../../bin
}



