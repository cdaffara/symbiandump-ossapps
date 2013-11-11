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
#Header files
HEADERS += ./inc/*.h \
           ../../inc/*.h \
           ../../../../hsdomainmodel/inc/hsdomainmodeldatastructures.h \
           ../../../../hsdomainmodel/inc/hsdatabase.h \
           ../../../../hsdomainmodel/inc/hspage.h \
           ../../../../hsdomainmodel/inc/hspagevisual.h \
           ../../../../hsdomainmodel/inc/hspagetoucharea.h \
           ../../../../hsdomainmodel/inc/hswidgethost.h \
           ../../../../hsdomainmodel/inc/hswidgethostvisual.h \
           ../../../../hsdomainmodel/inc/hswidgettoucharea.h \
           ../../../../hsdomainmodel/inc/hsscene.h \
           ../../../../hsdomainmodel/inc/hscontentservice.h \
           ../../../../hsdomainmodel/inc/hsshortcutservice.h \
           ../../../../hsdomainmodel/inc/hsshortcutservice_p.h \
           ../../../../hsdomainmodel/inc/hswallpaper.h \
           ../../../../hsdomainmodel/inc/hsbackuprestoreobserver.h \
           ../../../../hsdomainmodel/inc/hsgui.h \
           ../../../../hsdomainmodel/inc/hsidlewidget.h \
           ../../../../hsdomainmodel/inc/hsdocumentloader.h \ 
           ../../../../hsdomainmodel/inc/hsconfiguration.h \
           ../../../../hsutils/inc/*.h \
           ../../../../hsutils/tsrc/t_hsutils/inc/mocksysteminfo.h 
           


#Source files
SOURCES += ./src/*.cpp \
           ../../src/*.cpp \
           ../../../../hsdomainmodel/src/hscontentservice.cpp \
           ../../../../hsdomainmodel/src/hsshortcutservice.cpp \
           ../../../../hsdomainmodel/src/hsconfiguration.cpp \
						../../../../hsdomainmodel/src/hsidlewidget.cpp \
					../../../../hsdomainmodel/src/hsdocumentloader.cpp \
					../../../../hsdomainmodel/src/hspagevisual.cpp \
           ../../../../hsdomainmodel/src/hspagetoucharea.cpp \
           ../../../../hsdomainmodel/src/hswidgethostvisual.cpp \
           ../../../../hsdomainmodel/src/hswidgettoucharea.cpp \
           ../../../../hsdomainmodel/src/hswidgetpositioningonwidgetadd.cpp \           
           ../../../../hsutils/src/hsimagegridwidget.cpp \
           ../../../../hsutils/src/hsmenuevent.cpp \
           ../../../../hsutils/src/hsmenueventfactory.cpp \
           ../../../../hsutils/src/hsmenueventtransition.cpp \
           ../../../../hsutils/src/hspageindicator.cpp \
           ../../../../hsutils/src/hspageindicatoritem.cpp \
           ../../../../hsutils/src/hstrashbinwidget.cpp \
           ../../../../hsutils/src/hswidgetpositioningonorientationchange.cpp \
           ../../../../hsutils/src/hstitleresolver.cpp \
           ../../../../hsutils/src/hsdeviceinfolistener.cpp \
           ../../../../hsutils/tsrc/t_hsutils/src/mocksysteminfo.cpp \
           ../../../../hsutils/src/hsspinnerdialog.cpp \
           ../../../../hsutils/src/hswidgetpositioningonwidgetmove.cpp \
           ../../../../hsutils/src/hssnapline.cpp 
           