#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, version 2.1 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, 
# see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
#
# Description:
#

TEMPLATE = lib
TARGET = BedrockProvisioning
QT += core webkit

ROOT_DIR = $$PWD/..
include($$ROOT_DIR/browserui.pri)

#This is used to toggle behaviour of BEDROCKPROVISION_EXPORT
#between Q_DECL_EXPORT and Q_DECL_IMPORT
DEFINES+= BUILDING_BEDROCK_PROVISIONING

symbian: {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -TCB -DRM -AllFiles 
    TARGET.UID3 = 0x200267EA
    TARGET.VID = VID_DEFAULT
    
    bedrockprovisioning.sources = BedrockProvisioning.dll
    bedrockprovisioning.path = /sys/bin    
    provisioningtemplate.sources = ./template/200267EA.ini
    provisioningtemplate.path = /data/.config/Nokia
    
    DEPLOYMENT += bedrockprovisioning provisioningtemplate
    LIBS += -lefsrv
    
    isEmpty(BEDROCK_OUTPUT_DIR): {
        CONFIG(release, debug|release):BEDROCKPROVISIONING_OUTPUT_DIR=$$PWD/../WrtBuild/Release
        CONFIG(debug, debug|release):BEDROCKPROVISIONING_OUTPUT_DIR=$$PWD/../WrtBuild/Debug
    } else {
        BEDROCKPROVISIONING_OUTPUT_DIR=$$BEDROCK_OUTPUT_DIR
    }
    
} else {

    isEmpty(BEDROCK_OUTPUT_DIR): {
        CONFIG(release, debug|release):BEDROCKPROVISIONING_OUTPUT_DIR=$$PWD/../../../../WrtBuild/Release
        CONFIG(debug, debug|release):BEDROCKPROVISIONING_OUTPUT_DIR=$$PWD/../../../../WrtBuild/Debug
    } else {
        BEDROCKPROVISIONING_OUTPUT_DIR=$$BEDROCK_OUTPUT_DIR
    }
}

contains(br_layout, tenone ) {
    DEFINES += BROWSER_LAYOUT_TENONE
}

OBJECTS_DIR = $$BEDROCKPROVISIONING_OUTPUT_DIR/BedrockProvisioning/tmp
DESTDIR = $$BEDROCKPROVISIONING_OUTPUT_DIR/bin
MOC_DIR = $$BEDROCKPROVISIONING_OUTPUT_DIR/BedrockProvisioning/
RCC_DIR = $$BEDROCKPROVISIONING_OUTPUT_DIR/BedrockProvisioning/
TEMPDIR = $$BEDROCKPROVISIONING_OUTPUT_DIR/BedrockProvisioning/build

INCLUDEPATH += $$PWD/

CONFIG += dll

#
# Our headers, source
#
HEADERS += $$PWD/bedrockprovisioning.h \
           $$PWD/bedrockprovisioningglobal.h
SOURCES += $$PWD/bedrockprovisioning.cpp

CONFIG(gcov)   {
   LIBS +=   -lgcov
   QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
   message( "building for coverage statics" )
}


symbian:MMP_RULES += SMPSAFE
