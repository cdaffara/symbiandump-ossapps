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
TARGET = unifiedviewer
DEFINES += BUILD_UNI_VIEWER_DLL

DEPENDPATH += . src inc ../inc

INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../appengine/inc
INCLUDEPATH += ../msguiutils/inc
INCLUDEPATH += . ../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

#QMAKE_CXXFLAGS.ARMCC -= --no_hide_all

#UID3
TARGET.UID3 = 0x2002432B

# Capability
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1
TARGET.EPOCHEAPSIZE = 0x20000 0x2000000

CONFIG += hb
HB += hbfeedback

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/unifiedviewer.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(unifiedviewer.iby)"

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/unifiedviewer.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/unifiedviewer.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# Input
HEADERS += inc/unifiedviewer.h \
           inc/unicontentswidget.h \
           inc/univiewerfeeder.h \
           inc/univiewerfeeder_p.h \
           inc/univiewslidewidget.h \
           inc/uniscrollarea.h \
           ../inc/msgbaseview.h \
           inc/univiewerbodywidget.h \
           inc/univiewerdetailswidget.h \
           inc/univieweraddresswidget.h \
           inc/univieweraddresscontainer.h \
           inc/univiewerattachmentwidget.h \
           inc/univiewertextitem.h \
           inc/univiewerheadercontainer.h \
           inc/univiewerpixmapwidget.h \
           inc/univieweraudiowidget.h

SOURCES += src/unifiedviewer.cpp \
           src/unicontentswidget.cpp \
           src/univiewerfeeder.cpp \
           src/univiewerfeeder_p.cpp \
           src/univiewslidewidget.cpp \
           src/uniscrollarea.cpp \
           src/univiewerbodywidget.cpp \
           src/univiewerdetailswidget.cpp \
           src/univieweraddresswidget.cpp \
           src/univieweraddresscontainer.cpp \
           src/univiewerattachmentwidget.cpp \
           src/univiewertextitem.cpp \
           src/univiewerheadercontainer.cpp \
           src/univiewerpixmapwidget.cpp \
           src/univieweraudiowidget.cpp

RESOURCES += unifiedviewer.qrc

# Libs
LIBS = -letext \
       -lmsgs \
       -lconvergedmessageutils \
       -lappengine \
       -lunidatamodelloader \
       -lxqservice \
       -lQtContacts \
       -lmsguiutils \
       -lcentralrepository \
       -lthumbnailmanagerqt \
       -lxqserviceutil \
       -lsqldb

symbian:MMP_RULES += SMPSAFE
