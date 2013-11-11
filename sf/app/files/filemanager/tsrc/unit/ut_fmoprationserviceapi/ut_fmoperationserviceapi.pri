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


LIBS += -lfmbkupenginewrapper
LIBS += -lfmfiledialog

symbian { 
    LIBS += -lplatformenv \ 
            -lapmime \ 
            -lxqservice \ 
            -lxqutils \ 
            -lshareui \ 
            -lefsrv \ 
            -lapgrfx \ 
            -lcone \ 
            -ldirectorylocalizer \
            -lsysutil \
            -lbafl \
            -leikcore \
            -lafservice
}

# Input
HEADERS += src/ut_fmoprationserviceapi.h \
           src/testviewmanager.h \
           ../../../src/filemanager/src/fmdriverlistwidget.h \
#           ../../../src/filemanager/src/listviewitems.h \
           ../../../src/filemanager/src/fmfileview.h \
           ../../../src/filemanager/src/fmfilebrowsewidget.h \
           ../../../src/filemanager/src/fmdriverview.h \
           ../../../src/filemanager/src/fmmainwindow.h \
           ../../../src/filemanager/src/fmsplitview.h \
           ../../../src/filemanager/src/fmfindthread.h \
           ../../../src/filemanager/src/fmfindresultmodel.h \
           ../../../src/filemanager/src/fmfindview.h \
           ../../../src/filemanager/src/fmfindwidget.h \
           ../../../src/filemanager/src/fmviewmanager.h \
           ../../../src/filemanager/src/fmviewbase.h \
           ../../../src/filemanager/src/operationservice/fmoperationservice.h \
           ../../../src/filemanager/src/operationservice/fmoperationthread.h \
           ../../../src/filemanager/src/operationservice/fmoperationbase.h \
           ../../../src/filemanager/src/operationservice/fmoperationresultprocesser.h \
           ../../../src/filemanager/src/operationservice/fmoperationcopyormove.h \
           ../../../src/filemanager/src/operationservice/fmoperationviewdetails.h \
#           ../../../src/filemanager/src/operationservice/fmoperationmove.h \
           ../../../src/filemanager/src/operationservice/fmoperationremove.h \
           ../../../src/filemanager/src/operationservice/fmoperationformat.h \
           ../../../src/filemanager/src/backuprestore/fmbackupview.h \
           ../../../src/filemanager/src/backuprestore/fmbackupwidget.h \
           ../../../src/filemanager/src/backuprestore/fmrestoreview.h \
           ../../../src/filemanager/src/backuprestore/fmrestorewidget.h \
           ../../../src/filemanager/src/backuprestore/fmrestoreviewitem.h \
           ../../../src/filemanager/src/backuprestore/fmbackupconfigloader.h \
           ../../../src/filemanager/src/backuprestore/fmbackuprestorehandler.h \
           ../../../src/filemanager/src/backuprestore/fmdeletebackupview.h  \
           ../../../src/filemanager/src/backuprestore/fmdeletebackupwidget.h  \
           ../../../src/filemanager/src/components/fmcombinedquery.h \
           ../../../src/filemanager/src/components/fmdlgutils.h \
           ../../../src/filemanager/src/components/fmtimequery.h \
           ../../../src/filemanager/src/components/fmsingletextquery.h \
           ../../../src/filemanager/src/components/fmmultitextquery.h \
           ../../../src/filemanager/src/components/fmviewdetailsdialog.h \
           ../../../src/filemanager/src/components/fmviewdetailsitem.h \
           ../../../src/filemanager/src/components/fmdrivedetailstype.h \
           ../../../src/filemanager/src/components/fmmessagebox.h \
           ../../../src/filemanager/src/components/fmdialog.h \
           ../../../src/filemanager/src/components/fmdrivequery.h 
           
SOURCES += src/ut_fmoprationserviceapi.cpp \
           src/testviewmanager.cpp \
           ../../../src/filemanager/src/fmdriverlistwidget.cpp \
#           ../../../src/filemanager/src/listviewitems.cpp \
           ../../../src/filemanager/src/fmfileview.cpp \
           ../../../src/filemanager/src/fmfilebrowsewidget.cpp \
           ../../../src/filemanager/src/fmdriverview.cpp \
           ../../../src/filemanager/src/fmmainwindow.cpp \
           ../../../src/filemanager/src/fmsplitview.cpp \
           ../../../src/filemanager/src/fmfindthread.cpp \
           ../../../src/filemanager/src/fmfindresultmodel.cpp \
           ../../../src/filemanager/src/fmfindview.cpp \
           ../../../src/filemanager/src/fmfindwidget.cpp \
           ../../../src/filemanager/src/fmviewmanager.cpp \
           ../../../src/filemanager/src/fmviewbase.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationservice.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationthread.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationbase.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationresultprocesser.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationcopyormove.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationviewdetails.cpp \
           #../../../src/filemanager/src/operationservice/fmoperationmove.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationremove.cpp \
           ../../../src/filemanager/src/operationservice/fmoperationformat_s60.cpp \
           ../../../src/filemanager/src/backuprestore/fmbackupview.cpp \
           ../../../src/filemanager/src/backuprestore/fmbackupwidget.cpp \
           ../../../src/filemanager/src/backuprestore/fmrestoreview.cpp \
           ../../../src/filemanager/src/backuprestore/fmrestorewidget.cpp \
           ../../../src/filemanager/src/backuprestore/fmrestoreviewitem.cpp \
           ../../../src/filemanager/src/backuprestore/fmbackupconfigloader.cpp \
           ../../../src/filemanager/src/backuprestore/fmbackuprestorehandler.cpp \
           ../../../src/filemanager/src/backuprestore/fmdeletebackupview.cpp  \
           ../../../src/filemanager/src/backuprestore/fmdeletebackupwidget.cpp  \
           ../../../src/filemanager/src/components/fmcombinedquery.cpp \
           ../../../src/filemanager/src/components/fmdlgutils.cpp \
           ../../../src/filemanager/src/components/fmtimequery.cpp \
           ../../../src/filemanager/src/components/fmsingletextquery.cpp \
           ../../../src/filemanager/src/components/fmmultitextquery.cpp \
           ../../../src/filemanager/src/components/fmviewdetailsdialog.cpp \
           ../../../src/filemanager/src/components/fmviewdetailsitem.cpp \
           ../../../src/filemanager/src/components/fmdrivedetailstype.cpp \
           ../../../src/filemanager/src/components/fmmessagebox.cpp \
           ../../../src/filemanager/src/components/fmdialog.cpp \
           ../../../src/filemanager/src/components/fmdrivequery.cpp      