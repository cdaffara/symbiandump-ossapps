# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = lib
TARGET = nmailuiwidgets
CONFIG += hb
HB += hbfeedback
DEFINES += BUILD_NMAILUIWIDGETS_DLL
INCLUDEPATH += ./inc \
  ../../inc

HEADERS += inc/nmautofilllistview.h \
           inc/nmautofilllistviewitem.h \
           inc/nmailuiwidgetsheaders.h \
           inc/nmailuiwidgetsdef.h \
           inc/nmbaseviewscrollarea.h \
           inc/nmrecipientlineedit.h \
           inc/nmhtmllineedit.h \
           inc/nmeditortextedit.h \
           inc/nmattachmentlistwidget.h \
           inc/nmattachmentlistitem.h

SOURCES += src/nmautofilllistview.cpp \
           src/nmautofilllistviewitem.cpp \
           src/nmbaseviewscrollarea.cpp \
           src/nmrecipientlineedit.cpp \
           src/nmhtmllineedit.cpp \
           src/nmeditortextedit.cpp \
           src/nmattachmentlistwidget.cpp \
           src/nmattachmentlistitem.cpp

symbian*: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002C348
    LIBS += -lnmailbase \
            -lxqservice \
            -lxqserviceutil \
            -lqtcontacts \
            -lnmutilities

    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmailuiwidgets.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmailuiwidgets.def" \
      "$${LITERAL_HASH}endif"
    
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
}

win32 {
    DESTDIR = ../../bin
    LIBS += -L../../bin \
        -lnmailbase
}
       
RESOURCES += nmailuiwidgets.qrc

DOCML += resources/nmattachmentlistwidget.docml
