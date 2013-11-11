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

HEADERS += src/tonefetchermainwindow.h \
           src/tonefetcher.h \
           src/tonefetcherview.h \
           src/tonefetcherwidget.h \
           src/tonefetchermodel.h \
           tonefetcherengine/tonefetcherengine.h \
           inc/tonefetcherutils.h

SOURCES += src/tonefetchermainwindow.cpp \
           src/tonefetcher.cpp \
           src/tonefetcherview.cpp \
           src/tonefetcherwidget.cpp \
           src/tonefetchermodel.cpp \
           tonefetcherengine/tonefetcherengine.cpp \
           inc/tonefetcherutils.cpp \
           src/main.cpp 

symbian {
    HEADERS +=  tonefetcherengine/private/CTonePlayer.h \
                tonefetcherengine/private/CToneSelection.h \
                tonefetcherengine/private/MTonePlayingWatcher.h \
                tonefetcherengine/private/MToneSelectionWatcher.h \
                tonefetcherengine/private/tonefetcherengine_symbian.h
                


    SOURCES += tonefetcherengine/private/CTonePlayer.cpp \
               tonefetcherengine/private/CToneSelection.cpp \
               tonefetcherengine/private/tonefetcherengine_symbian.cpp
} else {
    HEADERS +=  tonefetcherengine/private/tonefetcherengine_stub.h
    SOURCES += tonefetcherengine/private/tonefetcherengine_stub.cpp
}


