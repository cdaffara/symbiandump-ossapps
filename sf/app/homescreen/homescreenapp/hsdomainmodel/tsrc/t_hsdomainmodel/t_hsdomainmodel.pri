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


HEADERS += ./inc/*.h

SOURCES += ./src/*.cpp

HEADERS += ../../../hsutils/inc/hspropertyanimationwrapper.h \
           ../../../hsutils/inc/hswidgetpositioningonorientationchange.h \
           ../../../hsutils/inc/hsmenueventfactory.h \
           ../../../hsutils/inc/hsmenuevent.h \
           ../../../hsutils/inc/hspageindicator.h \
           ../../../hsutils/inc/hspageindicatoritem.h \
           ../../../hsutils/inc/hstrashbinwidget.h \
           ../../../hsutils/inc/hssnapline.h \
           ./../../inc/hscontentservice.h \
           ./../../inc/hsdatabase.h  \
           ./../../inc/hsdomainmodel_global.h  \
           ./../../inc/hspage.h \
           ./../../inc/hspagevisual.h \
           ./../../inc/hspagetoucharea.h \
           ./../../inc/hspagenewwidgetlayout.h \
           ./../../inc/hsscene.h \
           ./../../inc/hsshortcutservice.h \
           ./../../inc/hsshortcutservice_p.h \
           ./../../inc/hswallpaper.h \
           ./../../inc/hswallpaperloader.h \
           ./../../inc/hswallpaperloaderthread.h \
           ./../../inc/hshostedwidgetfactory.h \
           ./../../inc/hswidgethost.h \
           ./../../inc/hswidgethostvisual.h \
           ./../../inc/hswidgettoucharea.h \
           ./../../inc/hsdomainmodeldatastructures.h \
           ./../../inc/hswidgetcomponentregistry.h \
           ./../../inc/hswidgetcomponent.h \
           ./../../inc/hswidgetcomponentdescriptor.h \
           ./../../inc/hsbackuprestoreobserver.h \
           ./../../inc/hsconfiguration.h \
           ./../../inc/hsgui.h \
           ./../../inc/hsidlewidget.h \
           ./../../inc/hsdocumentloader.h \
           ./../../inc/hswidgetpositioningonwidgetadd.h

SOURCES += ../../../hsutils/src/hswidgetpositioningonorientationchange.cpp \
           ../../../hsutils/src/hsmenueventfactory.cpp \
           ../../../hsutils/src/hsmenuevent.cpp \
           ../../../hsutils/src/hspageindicator.cpp \
           ../../../hsutils/src/hspageindicatoritem.cpp \
           ../../../hsutils/src/hstrashbinwidget.cpp \
           ../../../hsutils/src/hssnapline.cpp \
           ./../../src/hscontentservice.cpp \
           ./../../src/hsdatabase.cpp  \
           ./../../src/hspage.cpp  \
           ./../../src/hspagevisual.cpp \
           ./../../src/hspagetoucharea.cpp  \
           ./../../src/hspagenewwidgetlayout.cpp  \
           ./../../src/hsscene.cpp \
           ./../../src/hsshortcutservice.cpp \
           ./../../src/hswallpaper.cpp \
           ./../../src/hswallpaperloader.cpp \
           ./../../src/hswallpaperloaderthread.cpp \
           ./../../src/hshostedwidgetfactory.cpp \
           ./../../src/hswidgethost.cpp \
           ./../../src/hswidgethostvisual.cpp \
           ./../../src/hswidgettoucharea.cpp \
           ./../../src/hswidgetcomponentregistry.cpp \
           ./../../src/hswidgetcomponent.cpp \
           ./../../src/hswidgetcomponentdescriptor.cpp \
           ./../../src/hsbackuprestoreobserver.cpp \
           ./../../src/hsconfiguration.cpp \
           ./../../src/hsgui.cpp \
           ./../../src/hsidlewidget.cpp \
           ./../../src/hsdocumentloader.cpp \
           ./../../src/hswidgetpositioningonwidgetadd.cpp
           
symbian: {
HEADERS += ./../../inc/hsbackuprestoreobserver_p.h \
           ./../../inc/hsbackuprestoreactivecallback.h \
           $${EPOCROOT}epoc32/include/connect/abclient.h
SOURCES += ./../../src/hsbackuprestoreactivecallback.cpp
}

