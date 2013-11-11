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
TARGET = MessageStoreServer
DEPENDPATH += . inc src
INCLUDEPATH += . \
			../client/api \
			../common/inc \
			../../inc \
			../../../../inc


HEADERS += inc/BackupRestoreObserver.h \
           inc/ContainerStore.h \
           inc/ContainerStoreAccountTable.h \
           inc/ContainerStoreContainersTable.h \
           inc/ContainerStoreContentManager.h \
           inc/ContainerStoreDefs.h \
           inc/ContainerStoreDeleteHandler.h \
           inc/ContainerStoreEncryption.h \
           inc/ContainerStoreGeneralTable.h \
           inc/ContainerStoreMRUAddressTable.h \
           inc/ContainerStoreSearchHandler.h \
           inc/ContainerStoreSearchResultTable.h \
           inc/ContainerStoreSortingTable.h \
           inc/ContainerStoreTable.h \
           inc/ContainerStoreUtils.h \
           inc/EncryptableTable.h \
           inc/MessageStoreEncryptHandler.h \
           inc/MessageStoreServer.h \
           inc/MessageStoreSession.h \
           inc/MsgStoreInMemorySortRowSet.h \
           inc/MsgStoreSortResultRowSet.h \
           inc/MsgStoreStringSortResultRowSet.h \
           inc/ShutdownServer.h \
           inc/StoreDriveMonitor.h \
           inc/bufferedmessagecreator.h \
           inc/messagestoreutils.h

SOURCES += src/BackupRestoreObserver.cpp \
           src/ContainerStore.cpp \
           src/ContainerStoreAccountTable.cpp \
           src/ContainerStoreContainersTable.cpp \
           src/ContainerStoreContentManager.cpp \
           src/ContainerStoreDeleteHandler.cpp \
           src/ContainerStoreEncryption.cpp \
           src/ContainerStoreGeneralTable.cpp \
           src/ContainerStoreMRUAddressTable.cpp \
           src/ContainerStoreSearchHandler.cpp \
           src/ContainerStoreSearchResultTable.cpp \
           src/ContainerStoreSortingTable.cpp \
           src/ContainerStoreTable.cpp \
           src/ContainerStoreUtils.cpp \
           src/MessageStoreEncryptHandler.cpp \
           src/MessageStoreServer.cpp \
           src/MessageStoreSession.cpp \
           src/MsgStoreInMemorySortRowSet.cpp \
           src/MsgStoreSortResultRowSet.cpp \
           src/MsgStoreStringSortResultRowSet.cpp \
           src/ShutdownServer.cpp \
           src/StoreDriveMonitor.cpp \
           src/bufferedmessagecreator.cpp \
           src/messagestoreutils.cpp \
           ../common/src/PropertiesSerializer.cpp \
		   ../common/src/AccountsSerializer.cpp \
		   ../common/src/psintpropertyobserver.cpp

LIBS += -leuser \
		-lestor \
		-lefsrv \
		-lbafl \
		-ledbms \
		-lsysutil \
		-lcharconv \
		-lDebugLog \
		-lImsPointSecMonitor \
		-lplatformenv

symbian*: {

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_CLIENT_DLL
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x2001FDA3

    defBlock = \
	  "$${LITERAL_HASH}if defined(MARM)" \
	  "DEFFILE  eabi/messagestoreserver.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/messagestoreserver.def" \
      "$${LITERAL_HASH}endif"

    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}

win32 {
	DESTDIR = ../../../../bin
}
