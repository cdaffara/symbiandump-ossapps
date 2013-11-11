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

TEMPLATE = app
TARGET = ut_categorymodelutility

QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test

DEPENDPATH += .

QMAKE_EXTRA_TARGETS += test autotest

include ( ../../../common.pri )
include (../unit_common.pri)
include(../../../inc/inc.pri)
include(../../cpcategorymodel.pri)

LIBS += -lcpframework -lcplogger

CONFIG += Hb xml
DEFINES += BUILD_CPCATEGORY_GLOBAL

symbian*: { 
    TARGET.CAPABILITY = CAP_APPLICATION
    LIBS += -lfirstpluginforcpcategorymodel \
            -lsecondpluginforcpcategorymodel
           
    deploy.path = C:
    testdlls.sources += firstpluginforcpcategorymodel.dll \
                       secondpluginforcpcategorymodel.dll
    testdlls.path = /sys/bin
    
    testqtplugins.sources += ../testpluginsforcpcategorymodel/firstpluginforcpcategorymodel/qmakepluginstubs/firstpluginforcpcategorymodel.qtplugin \
                             ../testpluginsforcpcategorymodel/secondpluginforcpcategorymodel/qmakepluginstubs/secondpluginforcpcategorymodel.qtplugin
    testqtplugins.path = /resource/qt/plugins/controlpanel
    
    testqtcfgfile.sources += data/configfiletestformodelutility.cpcfg
    testqtcfgfile.path = /resource/qt/plugins/controlpanel/config                  
    
    DEPLOYMENT += testdlls  \
                  testqtplugins \
                  testqtcfgfile
}
HEADERS += src/*.h
SOURCES += src/*.cpp

symbian:MMP_RULES += SMPSAFE
