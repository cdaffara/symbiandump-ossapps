
#/* 
#* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#* All rights reserved.
#* This component and the accompanying materials are made available
#* under the terms of "Eclipse Public License v1.0"
#* which accompanies this distribution, and is available
#* at the URL "http://www.eclipse.org/legal/epl-v10.html".
#*
#* Initial Contributors:
#* Nokia Corporation - initial contribution.
#*
#* Contributors:
#* 
#* Description:
#*
#*/ 
TEMPLATE        = lib
CONFIG         += qt ecom hb
TARGET =	GlxExternalViewPlugin

TARGET.DEFAULT_DATA = org.nokia.mmdt.QViewPlugin/1.0
TARGET.OPAQUE_DATA = "<t>0x10000000</t><p>0x30000001</p><i>EMPXViewPluginPriorityNormal</i><f>0x0000001f</f>"


LIBS += -lhb.dll \
        -lqpluginframework.dll \
        -lglxexternalutility.dll \
        -lglxstatehandler.dll

INCLUDEPATH    += . \
                  /orbit/include \
                  ../inc \
                  inc \  
                  ../../inc \
                  ../../viewmanagement/statehandler/inc 
                  
HEADERS         += inc/GlxExternalViewPlugin.h \
                  ../inc/qviewplugin.h \

SOURCES         += src/GlxExternalViewPlugin.cpp

exportfile = "../../inc/glxplugincommandid.hrh APP_LAYER_PLATFORM_EXPORT_PATH(glxplugincommandid.hrh)"

BLD_INF_RULES.prj_exports += exportfile


symbian: { 
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
    TARGET.UID3 = 0x20000A02
    TARGET.EPOCALLOWDLLDATA = 1
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x14000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        41943040
    TARGET.CAPABILITY = ALL -TCB
    MMP_RULES += SMPSAFE
}