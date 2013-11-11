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
TARGET = messagestoreclient
DEPENDPATH += . api inc src
INCLUDEPATH += . \
			../common/inc \
			../../inc \
			../../../../inc

HEADERS += api/MsgStore.h \
           api/MsgStoreAccount.h \
           api/MsgStoreAddress.h \
           api/MsgStoreAPI.h \
           api/MsgStoreFolder.h \
           api/MsgStoreFolderUtils.h \
           api/MsgStoreMailBox.h \
           api/MsgStoreMailBoxObserver.h \
           api/MsgStoreMessage.h \
           api/MsgStoreMessagePart.h \
           api/MsgStoreObserver.h \
           api/MsgStorePropertyContainer.h \
           api/MsgStorePropertyContainerWithContent.h \
           api/MsgStorePropertyKeys.h \
           api/MsgStoreSearchCriteria.h \
           api/MsgStoreSortCriteria.h \
           api/MsgStoreSortResultIterator.h \
           api/MsgStoreTypes.h \
           api/MsgStoreWritablePropertyContainer.h \
           inc/MsgStoreObserverHandler.h \
           inc/MsgStorePropertyContainersArray.h \
           inc/MsgStoreSearchHandler.h \
           inc/MsgStoreSessionContext.h \
           inc/RMessageStoreSession.h \
           ../common/inc/AccountsSerializer.h \
           ../common/inc/PropertiesSerializer.h
           #MessageStoreClientServer.h
           
SOURCES += src/MessageStoreClient.cpp \
           src/MsgStore.cpp \
           src/MsgStoreAccount.cpp \
           src/MsgStoreAddress.cpp \
           src/MsgStoreFolder.cpp \
           src/MsgStoreFolderUtils.cpp \
           src/MsgStoreMailBox.cpp \
           src/MsgStoreMessage.cpp \
           src/MsgStoreMessagePart.cpp \
           src/MsgStoreObserverHandler.cpp \
           src/MsgStorePropertyContainer.cpp \
           src/MsgStorePropertyContainersArray.cpp \
           src/MsgStorePropertyContainerWithContent.cpp \
           src/MsgStoreSearchCriteria.cpp \
           src/MsgStoreSearchHandler.cpp \
           src/MsgStoreSessionContext.cpp \
           src/MsgStoreSortCriteria.cpp \
           src/MsgStoreSortResultIterator.cpp \
           src/MsgStoreWritablePropertyContainer.cpp \
           src/RMessageStoreSession.cpp \
           ../common/src/PropertiesSerializer.cpp \
           ../common/src/AccountsSerializer.cpp
           
LIBS += -leuser \
		-lestor \
		-lefsrv \
		-ldebuglog \
		-lMessageStoreServer           
		
symbian*: { 
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x2001FDA2
	
    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/messagestoreclient.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/messagestoreclient.def" \
      "$${LITERAL_HASH}endif"

    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}

win32 {
	DESTDIR = ../../../.../bin
}			
