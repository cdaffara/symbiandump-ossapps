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

HEADERS += ./inc/hscontentservice.h \
           ./inc/hsdomainmodeldatastructures.h \
           ./inc/hsdatabase.h  \
           ./inc/hsdomainmodel_global.h  \
           ./inc/hspage.h \
           ./inc/hspagevisual.h \
           ./inc/hspagenewwidgetlayout.h \
           ./inc/hsgui.h  \
           ./inc/hsscene.h \
           ./inc/hsshortcutservice.h \
           ./inc/hsshortcutservice_p.h \
           ./inc/hswallpaper.h \
           ./inc/hswallpaperloader.h \
           ./inc/hswallpaperloaderthread.h \
           ./inc/hshostedwidgetfactory.h \
           ./inc/hswidgethost.h \
           ./inc/hswidgethostvisual.h \
           ./inc/hswidgetcomponentregistry.h \
           ./inc/hswidgetcomponent.h \
           ./inc/hswidgetcomponentdescriptor.h \
           ./inc/hsbackuprestoreobserver.h \
           ./inc/hsconfiguration.h \
           ./inc/hswidgettoucharea.h \
           ./inc/hspagetoucharea.h \
           ./inc/hsidlewidget.h \
           ./inc/hsdocumentloader.h \
           ./inc/hswidgetpositioningonwidgetadd.h
        

SOURCES += ./src/hscontentservice.cpp \
           ./src/hsdatabase.cpp \
           ./src/hspage.cpp \
           ./src/hspagevisual.cpp \
           ./src/hspagenewwidgetlayout.cpp \
           ./src/hsgui.cpp \
           ./src/hsscene.cpp \
           ./src/hsshortcutservice.cpp \
           ./src/hswallpaper.cpp \
           ./src/hswallpaperloader.cpp \
           ./src/hswallpaperloaderthread.cpp \
           ./src/hshostedwidgetfactory.cpp \
           ./src/hswidgethost.cpp \
           ./src/hswidgethostvisual.cpp \
           ./src/hswidgetcomponentregistry.cpp \
           ./src/hswidgetcomponent.cpp \
           ./src/hswidgetcomponentdescriptor.cpp \
           ./src/hsbackuprestoreobserver.cpp \
           ./src/hsconfiguration.cpp \
           ./src/hswidgettoucharea.cpp \
           ./src/hspagetoucharea.cpp \
           ./src/hsidlewidget.cpp \
           ./src/hsdocumentloader.cpp \
           ./src/hswidgetpositioningonwidgetadd.cpp

symbian: {
HEADERS += ./inc/hsbackuprestoreobserver_p.h \
           ./inc/hsbackuprestoreactivecallback.h
SOURCES += ./src/hsbackuprestoreactivecallback.cpp
}
