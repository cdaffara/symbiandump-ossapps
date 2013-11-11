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
#
 
TEMPLATE = lib
CONFIG += plugin hb mobility qtservice
QTSERVICE.DESCRIPTOR = resource/nmregister.xml
MOBILITY = serviceframework
TARGET = nmregister
QT += core
LIBS += -lxqservice \
        -lnmailuiengine \
        -leuser \
        -llibc
symbian: {
TARGET.UID3=0x2002DD16
TARGET.VID = VID_DEFAULT
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
load(data_caging_paths)
pluginDep.sources = nmregister.dll
pluginDep.path = $$QT_PLUGINS_BASE_DIR
DEPLOYMENT += pluginDep

BLD_INF_RULES.prj_exports += "resource/nmregister.xml 			z:/private/2002DD16/nmregister.xml"
BLD_INF_RULES.prj_exports += "rom/nmregister.iby		CORE_APP_LAYER_IBY_EXPORT_PATH(nmregister.iby)"
BLD_INF_RULES.prj_exports += "rom/nmregisterlanguage.iby 		LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(nmregisterlanguage.iby)"
TARGET.EPOCALLOWDLLDATA = 1
MMP_RULES += SMPSAFE
}

TARGET.CAPABILITY = ALL -TCB
    
SOURCES += src/nmmailboxregisterinterface.cpp \
           src/nmmailboxregisterinterfaceplugin.cpp

HEADERS += inc/nmmailboxregisterinterface.h \
					 inc/nmmailboxregisterinterface_p.h \
					 inc/nmmailboxregisterinterfaceplugin.h

INCLUDEPATH += ../../inc

DEPENDPATH += .
TRANSLATIONS = nmregister.ts
