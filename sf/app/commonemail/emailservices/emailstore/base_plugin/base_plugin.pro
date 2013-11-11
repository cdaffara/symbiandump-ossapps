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

TEMPLATE = lib
TARGET = baseplugin
CONFIG += debug
DEPENDPATH += . inc src
DEFINES += BUILD_BASEPLUGIN_DLL
INCLUDEPATH += . \
               ../../../inc \
               $$APP_LAYER_SYSTEMINCLUDE
			

HEADERS += inc/BaseMrInfoObject.h \
           inc/BasePlugin.h \
           inc/baseplugincommonutils.h \
           inc/baseplugindelayedops.h \
           inc/baseplugindelayedopsprivate.h \
           inc/BasePluginPanic.h \
           inc/MailIterator.h \
	   inc/basepluginresourceloader.h  \
           inc/Map.h \
           inc/SafePointerArray.h \
           inc/Map.inl \
           inc/baseplugindef.h 
           
SOURCES += src/BaseMrInfoObject.cpp \
           src/BasePlugin.cpp \
           src/baseplugindelayedops.cpp \
           src/basepluginfetch.cpp \
           src/basepluginmisc.cpp \
           src/basepluginnotifications.cpp \
           src/BasePluginPanic.cpp \
           src/basepluginparts.cpp \
           src/baseplugintranslator.cpp \
           src/MailIterator.cpp \
	   src/basepluginresourceloader.cpp 

LIBS += -leuser \
		-lecom \
		-lcharconv \
		-lbafl \
		-lmessagestoreclient \
		-lfsfwcommonlib \
		-ldebuglog \
		-lcalinterimapi \
        -lefsrv \
        -lgdi
		
symbian*: { 
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x2001FDA9
	
	defBlock = \
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/baseplugin.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/baseplugin.def" \
      "$${LITERAL_HASH}endif"

    MMP_RULES += "SYSTEMINCLUDE /epoc32/include/ecom" \
                 defBlock \
                 SMPSAFE

    MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"
}

win32 {
	DESTDIR = ../../../bin
}	