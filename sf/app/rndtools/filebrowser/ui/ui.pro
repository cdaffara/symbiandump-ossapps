#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
TEMPLATE = app
TARGET = FileBrowser
DEPENDPATH += .
INCLUDEPATH += ./inc
load(hb.prf)
symbian:CONFIG -= symbian_i18n

HEADERS += ./inc/menuaction.h \
           ./inc/enginewrapper.h \
           ./inc/fbmainwindow.h \
           ./inc/fbdrivelistviewitem.h \
           ./inc/fbdrivemodel.h \
           ./inc/fbdriveentry.h \
           ./inc/fbdriveview.h \
           ./inc/fbfilelistviewitem.h \
           ./inc/fbfilemodel.h \
           ./inc/fbfileentry.h \
           ./inc/fbfileview.h \
           ./inc/filebrowsersortfilterproxymodel.h \
           ./inc/filebrowsersettings.h \
           ./inc/fbsettingsview.h \
           ./inc/fbeditorview.h \
           ./inc/fbsearchview.h \
           ./inc/fbattributesview.h \
           ./inc/fbfolderselectiondialog.h \
           ./inc/fbfolderselectorwrapper.h \
           ./inc/notifications.h
SOURCES += ./src/filebrowser.cpp \
           ./src/enginewrapper.cpp \
           ./src/fbmainwindow.cpp \
           ./src/fbdrivelistviewitem.cpp \
           ./src/fbdrivemodel.cpp \
           ./src/fbdriveentry.cpp \
           ./src/fbdriveview.cpp \
           ./src/fbfilelistviewitem.cpp \
           ./src/fbfilemodel.cpp \
           ./src/fbfileentry.cpp \
           ./src/fbfileview.cpp \
           ./src/filebrowsersortfilterproxymodel.cpp \
           ./src/fbsettingsview.cpp \
           ./src/fbeditorview.cpp \
           ./src/fbsearchview.cpp \
           ./src/fbattributesview.cpp \
           ./src/fbfolderselectiondialog.cpp \
           ./src/fbfolderselectorwrapper.cpp \
           ./src/notifications.cpp
RESOURCES += ./ui.qrc

symbian {
    INCLUDEPATH += ../
    LIBS += -leikcore \
            -lcommonengine \
            -ltzclient \
            -lapmime \
            -lcommonui \
            -leuser \
            -lapparc \
            -lcone \
            -lws32 \
            -lfbscli \
            -lImageConversion \
            -lPlatformEnv \
            -lapgrfx \
            -lefsrv \
            -lbafl \
            -lCentralRepository \
            -lmediaclientaudio \
            -lestor \
            -lgdi \
            -legul \
            -lsendui \
            -lmsgs \
            -lezlib \
            -lcdlengine \
            -lremconcoreapi \
            -lremconinterfacebase \
            -lsbeclient \
            -lhash \
            -letext \
            -lcharconv \
        #ifndef FILEBROWSER_LITE
            -lfilebrowserfileopclient
        #endif"

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS += ../engine/engine.h \
               ../engine/FBFileUtils.h \
               ../engine/FBFileOps.h \
               ../engine/FB.hrh \
               ../engine/FBStd.h \
               ../engine/FBTraces.h \
               ../engine/FBFolderSelector.h \
            #ifndef FILEBROWSER_LITE
               ../fileopclient/inc/FBFileOpClient.h
            #endif
    SOURCES += ../engine/engine.cpp \
               ../engine/FBFileUtils.cpp \
               ../engine/FBFileOps.cpp \
               ../engine/FBFolderSelector.cpp
    RSS_RULES += "group_name = \"RnD Tools\";"
    TARGET.CAPABILITY = NetworkServices LocalServices CAP_APPLICATION AllFiles DiskAdmin
    TARGET.UID3 = 0xE85157B0
    TARGET.VID = 0x101FB657 // Nokia
    TARGET.EPOCHEAPSIZE  = 0x20000 0x4000000  // Min 128Kb, Max 64Mb
    ICON = ../icons/qgn_menu_filebrowser.svg
} else {
    error("Only Symbian supported!")
}
	
