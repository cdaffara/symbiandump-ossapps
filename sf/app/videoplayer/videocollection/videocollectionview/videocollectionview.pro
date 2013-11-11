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

TEMPLATE = lib
CONFIG += hb qt ecomplugin mobility
TARGET = 

symbian: { 
    TARGET.UID2 = 0x10009D8D
    TARGET.UID3 = 0x200211FB
    BLD_INF_RULES.prj_exports += \
    	"rom/videocollectionview.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videocollectionview.iby)" \
    	"conf/videolistview.confml                      APP_LAYER_CONFML(videolistview.confml)" \
    	"conf/videoplayerservicelist.confml             APP_LAYER_CONFML(videoplayerservicelist.confml)" \
    	"conf/videolistview_2002BC63.crml               APP_LAYER_CRML(videolistview_2002BC63.crml)"
    
    TARGET.CAPABILITY = ALL -TCB -DRM

    # to enable resource access from Qt
    TARGET.EPOCALLOWDLLDATA = 1
    VERSION = 10.0
}

# mpx view plugin definitions:
# plugin interface name
SERVICE.INTERFACE_NAME = org.nokia.mmdt.QViewPlugin/1.0

# opaque data for resolving the correct plugin
SERVICE.CONFIGURATION = "<t>0x200211FC</t><p></p><i>EMPXViewPluginPriorityNormal</i><f>0x00000001</f>"

DEPENDPATH += .
INCLUDEPATH += . \
               ./inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE \
    		/epoc32/include/mw/hb/hbtools \
		/epoc32/include/mw

INCLUDEPATH += ../videocollectionwrapper/inc \
               ../inc \
               ../../inc \

# Input
HEADERS += inc/videocollectionviewplugin.h \
           inc/videolistview.h \
           inc/videolisttoolbar.h \
           inc/videolistmenu.h \
           inc/videolistwidget.h \
           inc/videocollectionviewutils.h \
           inc/videocollectionuiloader.h \
           inc/videolistselectiondialog.h \
           inc/videocollectioncenrepdefs.h \
           inc/videooperatorservice.h \
           inc/videooperatorservice_p.h

SOURCES += src/videocollectionviewplugin.cpp \
           src/videolistview.cpp \
           src/videolisttoolbar.cpp \
           src/videolistmenu.cpp \
           src/videolistwidget.cpp \
           src/videocollectionviewutils.cpp \
           src/videocollectionuiloader.cpp \
           src/videolistselectiondialog.cpp \
           src/videooperatorservice.cpp \
           src/videooperatorservice_p.cpp

DOCML += data/collectionview.docml \
	 data/videolistselectiondialog.docml

RESOURCES += data/videocollectionview.qrc

LIBS += -lmpxviewframeworkqt.dll \
        -lvideocollectionwrapper.dll \
        -lxqserviceutil.dll \
        -lvideoplayerengine.dll \
        -lflogger.dll \
        -lxqsettingsmanager.dll \
        -lws32 \
        -lapparc \
        -lapgrfx \
        -lcone

symbian: {
    # Seems to be no way to get these on top of file
    mmpBlock = "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "$${LITERAL_HASH}include <data_caging_paths.hrh>" \
        "$${LITERAL_HASH}include <bldvariant.hrh>"
    MMP_RULES += mmpBlock SMPSAFE
}
