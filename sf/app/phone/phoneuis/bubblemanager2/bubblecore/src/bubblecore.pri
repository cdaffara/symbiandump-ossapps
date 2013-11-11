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
#
HEADERS += ../inc/bubblemanager2.h \
    src/bubbleimagewidget.h \
    src/bubbleheader.h \
    src/bubbleutils.h \
    src/bubbleconferenceheader.h \
    src/bubbleparticipantlistitem.h \
    src/bubbleparticipantlistmodel.h \
    src/bubblecontainerwidget.h \
    src/bubbleheadingwidget.h \
    src/bubblewidgetmanager.h \
    src/bubblehandler.h \
    src/bubbleexpandedhandler.h \
    src/bubblecollapsedhandler.h \
    src/bubbleconferencehandler.h \
    src/bubbleimagemanager.h \
    src/bubbleimagemanager_p.h \
    src/bubbleeffecthandler.h \
    src/bubblebutton.h


SOURCES += src/bubblemanager2.cpp \
    src/bubbleheader.cpp \
    src/bubbleimagewidget.cpp \
    src/bubbleutils.cpp \
    src/bubbleconferenceheader.cpp \
    src/bubbleparticipantlistitem.cpp \
    src/bubbleparticipantlistmodel.cpp \
    src/bubblecontainerwidget.cpp \
    src/bubbleheadingwidget.cpp \
    src/bubblewidgetmanager.cpp \
    src/bubblehandler.cpp \
    src/bubbleexpandedhandler.cpp \
    src/bubblecollapsedhandler.cpp \
    src/bubbleconferencehandler.cpp \
    src/bubbleimagemanager.cpp \
    src/bubbleeffecthandler.cpp \
    src/bubblebutton.cpp

symbian {
    SOURCES += src/bubbleimagemanager_p.cpp
} else:win32 {
    SOURCES += src/bubbleimagemanager_p_stub.cpp
}
