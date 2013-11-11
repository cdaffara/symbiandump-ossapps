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
# Header files
HEADERS += ./inc/*.h \
    ../../inc/*.h  \
    ../../../../hsdomainmodel/inc/hsdomainmodeldatastructures.h \
    ../../../../hsdomainmodel/inc/hsdatabase.h \
    ../../../../hsdomainmodel/inc/hspage.h \
    ../../../../hsdomainmodel/inc/hspagetoucharea.h \
    ../../../../hsdomainmodel/inc/hswidgethost.h \
    ../../../../hsdomainmodel/inc/hswidgethostvisual.h \
    ../../../../hsdomainmodel/inc/hswidgettoucharea.h \
    ../../../../hsdomainmodel/inc/hsscene.h \
    ../../../../hsdomainmodel/inc/hswallpaper.h \
    ../../../../hsdomainmodel/inc/hsbackuprestoreobserver.h \
    ../../../../hsdomainmodel/inc/hsgui.h \
    ../../../../hsdomainmodel/inc/hsidlewidget.h \
    ../../../../hsdomainmodel/inc/hsdocumentloader.h \
    ../../../../hsdomainmodel/inc/hsconfiguration.h 
 
    

HEADERS -= ../../../hsdomainmodel/inc/hscontentservice.h
HEADERS -= ../../../hsdomainmodel/inc/hsshortcutservice.h

# Source files
SOURCES += ./src/*.cpp \
    ../../src/*.cpp \
    ../../../../hsdomainmodel/src/hsconfiguration.cpp \
    ../../../../hsdomainmodel/src/hspagetoucharea.cpp \
    ../../../../hsdomainmodel/src/hswidgettoucharea.cpp \
    ../../../../hsdomainmodel/src/hswidgethostvisual.cpp \
    ../../../../hsdomainmodel/src/hsidlewidget.cpp \
     ../../../../hsdomainmodel/src/hsdocumentloader.cpp 