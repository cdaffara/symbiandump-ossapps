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

DEFINES += XQPLUGIN_H
DEFINES += XQSERVICEABSTRACTSERVICE_H
DEFINES += XQSERVICEUTIL_H
    
CONFIG += qtestlib \
    Hb \
    symbian_test

LIBS += -lestor.dll \
    -lfbscli.dll \
    -lbitgdi.dll \
    -lmpxviewframeworkqt.dll \
    -lxqplugins.dll \
    -lxqservice.dll

HEADERS_TEMP = \
	inc/videoserviceurifetch.h \	
	inc/videoservicebrowse.h \
	inc/xqserviceutilstub.h \
	inc/xqserviceproviderstub.h \
	inc/xqsettingsmanagerstub.h \
    inc/testobjectstore.h \
    inc/hbpopup.h \
    inc/hbabstractitemview.h \
    inc/hbdocumentloader.h \
    inc/hbaction.h \
    inc/hbcheckbox.h \
    inc/hbdeviceprofile.h \
    inc/hbdialog.h \
    inc/hbnotificationdialog.h \
    inc/hbglobal.h \
    inc/hbgroupbox.h \
    inc/hbinputdialog.h \
    inc/hbinstance.h \
    inc/hblabel.h \
    inc/hblistwidget.h \
    inc/hblistview.h \
    inc/hblistviewitem.h \
    inc/hbmainwindow.h \
    inc/hbmenu.h \
    inc/hbmenudata.h \
    inc/hbmessagebox.h \
    inc/hbmessageboxdata.h \
    inc/hbnamespace.h \
    inc/hbpushbutton.h \
    inc/hbscrollbar.h \
    inc/hbstackedwidget.h \
    inc/hbstyleloader.h \
    inc/hbtoolbar.h \
    inc/hbtoolbarextension.h \
    inc/hbwidget.h \
    inc/hbview.h \
    inc/hbeffect.h \
    inc/videocollectionexport.h \
    inc/videocollectionuiloaderdata.h \
    inc/videocollectionviewutilsdata.h \
    inc/videocollectionwrapperdata.h \
    inc/videolistdatamodeldata.h \
    inc/videolistselectiondialogdata.h \
    inc/videolistwidgetdata.h \
    inc/videolistviewdata.h \
    inc/videolisttoolbardata.h \
    inc/videoplayerappexport.h \
    inc/videoproxymodeldata.h \
    inc/videothumbnailtestdata.h \
    inc/videoactivitystate.h \
    inc/xqserviceutilxtra.h \
    inc/videooperatorservicedata.h \
    inc/videooperatorservice_pdata.h \
    # We need stub for this because it has static data and HW compilation of moc file fails because of that.
    inc/videoservices.h \
    ../../videocollectionview/inc/videocollectionuiloader.h \
    ../../videocollectionview/inc/videocollectionviewutils.h \
    ../../videocollectionview/inc/videolistselectiondialog.h \
    ../../videocollectionview/inc/videolistwidget.h \
    ../../videocollectionview/inc/videolistview.h \
    ../../videocollectionview/inc/videolistmenu.h \
    ../../videocollectionview/inc/videolisttoolbar.h \
    ../../videocollectionwrapper/inc/videolistdatamodel.h \
    ../../videocollectionwrapper/inc/videocollectionwrapper.h \
    ../../videocollectionwrapper/inc/videoproxymodelgeneric.h \
    ../../videocollectionwrapper/inc/videoproxymodelallvideos.h \
    ../../videocollectionwrapper/inc/videoproxymodelcollections.h \
    ../../videocollectionwrapper/inc/videoproxymodelcontent.h \
    ../../videocollectionwrapper/inc/videothumbnaildata.h \
    ../../videocollectionview/inc/videooperatorservice.h \
    ../../videocollectionview/inc/videooperatorservice_p.h

SOURCES_TEMP = \
    src/xqserviceprovider.cpp \
    src/testobjectstore.cpp \
    src/hbdocumentloader.cpp \
    src/hbaction.cpp \
    src/hbcheckbox.cpp \
    src/hbdialog.cpp \
    src/hbnotificationdialog.cpp \
    src/hbinputdialog.cpp \
    src/hbglobal.cpp \
    src/hblabel.cpp \
    src/hblistview.cpp \
    src/hblistviewItem.cpp \
    src/hbmainwindow.cpp \
    src/hbmenu.cpp \
    src/hbmessagebox.cpp \
    src/hbscrollbar.cpp \
    src/hbstackedwidget.cpp \
    src/hbstyleloader.cpp \
    src/hbtoolbar.cpp \
    src/hbtoolbarextension.cpp \
    src/hbwidget.cpp \
    src/hbview.cpp \
    src/hbeffect.cpp \
    src/xqserviceutil.cpp \
    src/xqserviceutilxtra.cpp \
    src/videocollectionuiloader.cpp \
    src/videocollectionviewutils.cpp \
    src/videolistselectiondialog.cpp \
    src/videolistwidget.cpp \
    src/videolistview.cpp \
    src/videolistmenu.cpp \
    src/videolisttoolbar.cpp \
    src/videolistdatamodel.cpp \
    src/videocollectionwrapper.cpp \
    src/videoproxymodelgeneric.cpp \
    src/videoproxymodelallvideos.cpp \
    src/videoproxymodelcollections.cpp \
    src/videoproxymodelcontent.cpp \
    src/videothumbnaildata.cpp \
    src/videoservices.cpp \
    src/videoserviceurifetch.cpp \
    src/videoservicebrowse.cpp \ 
    src/videoactivitystate.cpp \
    src/videooperatorservice.cpp \
    src/videooperatorservice_p.cpp \
	src/videocollectionwrapperdata.cpp \
    src/xqsettingsmanagerstub.cpp

HEADERS += $$find(HEADERS_TEMP, ^(?!.*$$TESTEDCLASS).*$)
SOURCES += $$find(SOURCES_TEMP, ^(?!.*$$TESTEDCLASS).*$)
