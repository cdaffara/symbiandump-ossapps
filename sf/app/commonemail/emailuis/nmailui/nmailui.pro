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
CONFIG += debug
CONFIG += hb
CONFIG += service
CONFIG += no_icon

SERVICE.FILE = service_conf.xml

TEMPLATE = app
TARGET = nmailui
DEPENDPATH += .
INCLUDEPATH += . \
    ./inc \
    ../../inc \
    
DEFINES += UI_EXTENSION_INTERFACE
QT += webkit
QT += network
MOC_DIR = moc

#the following is for splash screen in phone
BLD_INF_RULES.prj_exports += "resources/mail_messagelist.splashml   /epoc32/data/z/resource/hb/splashml/mail_messagelist.splashml" \
                             "resources/mail_messagelist.docml      /epoc32/data/z/resource/hb/splashml/mail_messagelist.docml" \
                             "resources/mail_editor.splashml        /epoc32/data/z/resource/hb/splashml/mail_editor.splashml" \
                             "resources/mail_editor.docml           /epoc32/data/z/resource/hb/splashml/mail_editor.docml" \
                             "resources/mail_viewer.splashml        /epoc32/data/z/resource/hb/splashml/mail_viewer.splashml" \
                             "resources/mail_viewer.docml           /epoc32/data/z/resource/hb/splashml/mail_viewer.docml"

#the following is for splash screen in winscw_udeb
BLD_INF_RULES.prj_exports += "resources/mail_messagelist.splashml   /epoc32/release/winscw/udeb/z/resource/hb/splashml/mail_messagelist.splashml" \
                             "resources/mail_messagelist.docml      /epoc32/release/winscw/udeb/z/resource/hb/splashml/mail_messagelist.docml" \
                             "resources/mail_editor.splashml        /epoc32/release/winscw/udeb/z/resource/hb/splashml/mail_editor.splashml" \
                             "resources/mail_editor.docml           /epoc32/release/winscw/udeb/z/resource/hb/splashml/mail_editor.docml" \
                             "resources/mail_viewer.splashml        /epoc32/release/winscw/udeb/z/resource/hb/splashml/mail_viewer.splashml" \
                             "resources/mail_viewer.docml           /epoc32/release/winscw/udeb/z/resource/hb/splashml/mail_viewer.docml"
                             
#the following is for splash screen in winscw_urel
BLD_INF_RULES.prj_exports += "resources/mail_messagelist.splashml   /epoc32/release/winscw/urel/z/resource/hb/splashml/mail_messagelist.splashml" \
                             "resources/mail_messagelist.docml      /epoc32/release/winscw/urel/z/resource/hb/splashml/mail_messagelist.docml" \
                             "resources/mail_editor.splashml        /epoc32/release/winscw/urel/z/resource/hb/splashml/mail_editor.splashml" \
                             "resources/mail_editor.docml           /epoc32/release/winscw/urel/z/resource/hb/splashml/mail_editor.docml" \
                             "resources/mail_viewer.splashml        /epoc32/release/winscw/urel/z/resource/hb/splashml/mail_viewer.splashml" \
                             "resources/mail_viewer.docml           /epoc32/release/winscw/urel/z/resource/hb/splashml/mail_viewer.docml"
                             
LIBS += -leuser
LIBS += -llibc
LIBS += -lnmailbase
LIBS += -lnmailuiengine
LIBS += -lnmailuiwidgets
LIBS += -lnmsettingui
LIBS += -lnmutilities
LIBS += -ltstaskmonitorclient
LIBS += -lafservice

symbian*: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x200255BA
    TARGET.CAPABILITY = CAP_APPLICATION

    # TARGET.EPOCSTACKSIZE = 0x6000
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x1000000
    LIBS += -lxqservice \
        -lxqserviceutil \
        -lqtcontacts \
        -lfmfiledialog

    MMP_RULES += SMPSAFE
}

HEADERS += inc/nmeditorcontent.h \
    inc/nmeditorheader.h \
    inc/nmuiheaders.h \
    inc/nmmailboxlistview.h \
    inc/nmeditorview.h \
    inc/nmviewerview.h \
    inc/nmviewerwebview.h \
    inc/nmviewerheader.h \
    inc/nmviewerviewnetmanager.h \
    inc/nmviewerviewnetreply.h \
    inc/nmmessagelistview.h \
    inc/nmmessagesearchlistview.h \
    inc/nmbaseview.h \
    inc/nmapplication.h \
    inc/nmmailboxlistviewitem.h \
    inc/nmmessagelistviewitem.h \
    inc/nmuiviewids.h \
    inc/nmaction.h \
    inc/nmactionobserver.h \
    inc/nmactionrequest.h \
    inc/nmactionresponse.h \
    inc/nmuiextensionmanager.h \
    inc/nmuiextensioninterface.h \
    inc/nmuidef.h \
    inc/nmrecipientfield.h \
    inc/nmsendserviceinterface.h \
    inc/nmuriserviceinterface.h \
    inc/nmmailboxselectiondialog.h \
    inc/nmuidocumentloader.h \
    inc/nmmailboxserviceinterface.h \
    inc/nmviewerserviceinterface.h \
    inc/nmutilities.h \
    inc/nmattachmentlist.h \
    inc/nmeditortextdocument.h \
    inc/nmattachmentmanager.h \
    inc/nmattachmentpicker.h \
    inc/nmattachmentfetchobserver.h \
    inc/nmuieffects.h

SOURCES += src/nmeditorcontent.cpp \
    src/nmeditorheader.cpp \
    src/nmviewerview.cpp \
    src/nmviewerwebview.cpp \
    src/nmviewerheader.cpp \
    src/nmviewerviewnetmanager.cpp \
    src/nmviewerviewnetreply.cpp \
    src/nmmessagelistview.cpp \
    src/nmmessagesearchlistview.cpp \
    src/nmmailboxlistview.cpp \
    src/nmeditorview.cpp \
    src/nmbaseview.cpp \
    src/main.cpp \
    src/nmapplication.cpp \
    src/nmmailboxlistviewitem.cpp \
    src/nmmessagelistviewitem.cpp \
    src/nmuiextensionmanager.cpp \
    src/nmrecipientfield.cpp \
    src/nmsendserviceinterface.cpp \
    src/nmuriserviceinterface.cpp \
    src/nmmailboxselectiondialog.cpp \
    src/nmuidocumentloader.cpp \
    src/nmmailboxserviceinterface.cpp \
    src/nmviewerserviceinterface.cpp \
    src/nmutilities.cpp \
    src/nmattachmentlist.cpp \
    src/nmeditortextdocument.cpp \
    src/nmattachmentmanager.cpp \
    src/nmattachmentpicker.cpp \
    src/nmuieffects.cpp    

RESOURCES += nmailui.qrc
RESOURCES += nmmessagelistviewitem.qrc
RESOURCES += nmviewerheader.qrc
TRANSLATIONS = mail.ts

DOCML += conf/nmeditorview.docml \
         conf/nmmailboxlistview.docml \
         conf/nmmailboxselectiondialog.docml \
         conf/nmmailviewer.docml \
         conf/nmmessagelistview.docml \
         conf/nmmessagesearchlistview.docml
