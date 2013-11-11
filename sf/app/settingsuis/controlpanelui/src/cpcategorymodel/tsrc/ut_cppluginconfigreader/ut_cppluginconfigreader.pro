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

QT += testlib
CONFIG += hb qtestlib
CONFIG += symbian_test

TEMPLATE = app
TARGET = ut_cppluginconfigreader
QMAKE_EXTRA_TARGETS += test autotest

DEPENDPATH += .
INCLUDEPATH += . ../../src\
	        ../../../inc 

include (../unit_common.pri)
include(../../../inc/inc.pri)
include(../../cpcategorymodel.pri)

LIBS += -lcpframework
LIBS += -lcpcategorymodel
DEFINES += BUILD_CPCATEGORY_GLOBAL

symbian { 
    
    deploy.path = C:
    configfiles.sources += data/pluginfortest.cpcfg \
    					   data/emptypluginfortest.cpcfg \
    					   data/pluginfilewithwrongchildtag.cpcfg \
    					   data/pluginfilewithemptychildelement.cpcfg \
    					   data/pluginfilewithwrongplugintag.cpcfg \
    					   data/pluginfilewithoutattrs.cpcfg \
    					   data/pluginfilewithwrongdesctag.cpcfg
    configfiles.path = /resource/qt/plugins/controlpanel/config
    DEPLOYMENT += configfiles
    
    # This is for new exporting system coming in garden
    for(configfile, configfiles.sources):BLD_INF_RULES.prj_exports += "./$$configfile $$deploy.path$$configfiles.path/$$basename(configfile)"
}

HEADERS += src/*.h
SOURCES += src/*.cpp

symbian:MMP_RULES += SMPSAFE
