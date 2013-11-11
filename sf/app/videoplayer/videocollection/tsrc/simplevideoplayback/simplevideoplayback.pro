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

TEMPLATE = lib
CONFIG += hb qt ecomplugin
TARGET = simplevideoplayback

# MPX view plugin interface name
SERVICE.INTERFACE_NAME = org.nokia.mmdt.QViewPlugin/1.0

# Opaque data for resolving the correct plugin
SERVICE.CONFIGURATION = "<t>0x20024338</t><p></p><i>EMPXViewPluginPriorityNormal</i><f>0x00000002</f>"

INCLUDEPATH += inc \
               ../../../inc
DEPENDPATH += inc
VPATH += src

HEADERS += svpbdocumentloader.h \
           svpbengine.h \
           svpbnativewindow.h \
           svpbplugin.h \
           svpbsurfacecontainer.h \
           svpbview.h \
           trace.h

SOURCES += svpbdocumentloader.cpp \
           svpbengine.cpp \
           svpbnativewindow.cpp \
           svpbplugin.cpp \
           svpbview.cpp

DOCML += resources/simplevideoplayback.docml

RESOURCES = simplevideoplayback.qrc

LIBS += -lcone \
        -lmediaclientvideodisplay \
        -lmpxcommon \
        -lmpxplaybackutility \
        -lmpxviewframeworkqt \
        -lws32

symbian:
{
    TARGETTYPE_OVERRIDE = PLUGIN
    # KMpxVideoPluginDllPlaybackUid
    TARGET.UID3 = 0x20024335
    TARGET.CAPABILITY = ALL -TCB -DRM
    TARGET.EPOCALLOWDLLDATA = 1
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    BLD_INF_RULES.prj_exports += "rom/simplevideoplayback.iby CORE_APP_LAYER_IBY_EXPORT_PATH(simplevideoplayback.iby)"
    MMP_RULES += SMPSAFE
}
