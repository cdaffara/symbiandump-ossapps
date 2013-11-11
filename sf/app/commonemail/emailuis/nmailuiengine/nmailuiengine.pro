# Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

CONFIG += debug
CONFIG += hb

TEMPLATE = lib

TARGET = nmailuiengine

QT += core

DEFINES += BUILD_UIENGINE_DLL

MOC_DIR = ./moc

INCLUDEPATH += ./inc \
               ../../inc \

HEADERS += inc/nmmessagecreationoperation.h \
           inc/nmuiengine.h \
           inc/nmdatamanager.h \
           inc/nmmailboxlistmodel.h \
           inc/nmmailboxlistmodelitem.h \
           inc/nmmailboxmetadata.h \
           inc/nmdatapluginfactory.h \
           inc/nmfolderlistmodel.h \
           inc/nmfoldermetadata.h \
           inc/nmmessagelistmodelitem.h \
           inc/nmmessagelistmodel.h \
           inc/nmuienginedef.h \
           inc/nmdataplugininterface.h \
           inc/nmicons.h \
           inc/nmbaseclientplugin.h \
           inc/nmoperation.h \
           inc/nmaddattachmentsoperation.h \
           inc/nmmessagesendingoperation.h \
           inc/nmstoreenvelopesoperation.h \
           inc/nmapplicationstateinterface.h 

SOURCES += src/nmuiengine.cpp \
           src/nmdatamanager.cpp \
           src/nmmailboxlistmodel.cpp \
           src/nmmailboxlistmodelitem.cpp \
           src/nmmailboxmetadata.cpp \
           src/nmfolderlistmodel.cpp \
           src/nmfoldermetadata.cpp \
           src/nmmessagelistmodelitem.cpp \
           src/nmmessagelistmodel.cpp \
           src/nmdatapluginfactory.cpp \
           src/nmicons.cpp \
           src/nmbaseclientplugin.cpp \
           src/nmaddattachmentoperation.cpp \
           src/nmoperation.cpp
    
LIBS += -leuser
LIBS += -llibc
    
symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x20026763
    LIBS += -lnmailbase \
        -lxqserviceutil 

    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmailuiengine.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmailuiengine.def" \
      "$${LITERAL_HASH}endif"
    
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}

RESOURCES += nmailuiengine.qrc

# End of file.
