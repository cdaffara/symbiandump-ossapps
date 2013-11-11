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
TARGET = RfsMsgStorePlugin
DEPENDPATH += . inc src
INCLUDEPATH += . \
				../../../../inc


HEADERS += inc/RfsMsgStorePlugin.h

SOURCES += src/RfsMsgStorePlugin.cpp \
		src/RfsMsgStoreProxy.cpp

#RESOURCES += RfsMsgStorePlugin.qrc

LIBS += -leuser \
	-lecom \
	-lefsrv \
	-lplatformenv \
	-lbafl \
	-lestor

symbian*: {

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN
    TARGET.UID2 = 0x10009D8D
    TARGET.UID3 = 0x2000C8D3

	defBlock = \
	  "$${LITERAL_HASH}if defined(MARM)" \
	  "DEFFILE  eabi/rfsmsgstoreplugin.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/rfsmsgstoreplugin.def" \
      "$${LITERAL_HASH}endif"

    MMP_RULES += defBlock
    MMP_RULES += "SYSTEMINCLUDE /epoc32/include/ecom"
    MMP_RULES += SMPSAFE
}

win32 {
	DESTDIR = ../../../../bin
}