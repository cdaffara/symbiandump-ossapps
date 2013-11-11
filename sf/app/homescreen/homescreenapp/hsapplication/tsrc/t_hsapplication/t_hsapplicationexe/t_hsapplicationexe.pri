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


win32:{
    HEADERS += ./inc/t_hsapplication.h \
               ../../../inc/hshomescreen.h \
               ../../../inc/hsstatemachine.h

    SOURCES += ./src/t_hsapplication.cpp \
               ./src/testhomescreen.cpp \
               ./src/t_hsstatemachine.cpp \
               ../../../src/hshomescreen.cpp \
               ../../../src/hsstatemachine.cpp
}

symbian:{
    HEADERS += ./inc/*.h \
               ./../../../inc/hshomescreenclientserviceprovider.h \
               ./../../../inc/hshomescreen.h \
               ./../../../inc/hsstatemachine.h \
               ./../../../inc/hsrecoverymanager.h \
               ./../../../../hsdomainmodel/inc/hsdatabase.h \
               ./../../../../hsdomainmodel/inc/hsbackuprestoreobserver.h \
               ./../../../../hsdomainmodel/inc/hsbackuprestoreobserver_p.h \
               ./../../../../hsdomainmodel/inc/hsshortcutservice.h \
               ./../../../../hsdomainmodel/inc/hsshortcutservice_p.h \
               ./../../../../hsdomainmodel/inc/hswidgetcomponentdescriptor.h \
               ./../../../../hsdomainmodel/inc/hscontentservice.h \
               ./../../../../hsdomainmodel/inc/hsconfiguration.h \
               ./../../../../hsdomainmodel/inc/hsgui.h \
               ./../../../../hsdomainmodel/inc/hspage.h \
               ./../../../../hsdomainmodel/inc/hspagevisual.h \
               ./../../../../hsdomainmodel/inc/hspagetoucharea.h \
               ./../../../../hsdomainmodel/inc/hsscene.h \
               ./../../../../hsdomainmodel/inc/hswallpaper.h \
               ./../../../../hsdomainmodel/inc/hswidgetcomponent.h \
               ./../../../../hsdomainmodel/inc/hswidgetcomponentregistry.h \
               ./../../../../hsdomainmodel/inc/hswidgethost.h \
               ./../../../../hsdomainmodel/inc/hswidgethostvisual.h \
               ./../../../../hsdomainmodel/inc/hswidgettoucharea.h \
               ./../../../../hsdomainmodel/inc/hsbackuprestoreactivecallback.h \
               ./../../../../hsutils/inc/hsspinnerdialog.h \
               ./../../../../hsutils/inc/hssnapline.h        

    SOURCES += ./src/*.cpp \
               ./../../../src/hshomescreenclientserviceprovider.cpp \
               ./../../../src/hshomescreen.cpp \
               ./../../../src/hsstatemachine.cpp \
               ./../../../src/hsrecoverymanager.cpp \
               ./../../../../hsdomainmodel/src/hswidgetcomponentdescriptor.cpp \
               ./../../../../hsdomainmodel/src/hspagetoucharea.cpp \
               ./../../../../hsdomainmodel/src/hswidgettoucharea.cpp \
               ./../../../../hsdomainmodel/src/hsshortcutservice.cpp \
               ./../../../../hsdomainmodel/src/hsdatabase.cpp \
               ./../../../../hsdomainmodel/src/hsbackuprestoreobserver.cpp \
               ./../../../../hsdomainmodel/src/hsbackuprestoreactivecallback.cpp \
               ./../../../../hsdomainmodel/src/hswidgetpositioningonwidgetadd.cpp \
               ./../../../../hsutils/src/hsmenueventfactory.cpp \
               ./../../../../hsutils/src/hswidgetpositioningonorientationchange.cpp \
               ./../../../../hsutils/src/hswidgetpositioningonwidgetmove.cpp \
               ./../../../../hsutils/src/hsmenueventtransition.cpp \
               ./../../../../hsutils/src/hsmenuevent.cpp \
               ./../../../../hsutils/src/hssnapline.cpp               
}
