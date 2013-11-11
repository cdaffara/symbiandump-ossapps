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
# Description: Test Case Project Definition for UnieditorPluginLoader
#

QT += testlib
QT -= gui

CONFIG += hb
CONFIG += symbian_test
CONFIG += qtestlib

TEMPLATE = app
TARGET = testunieditorpluginloader

INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

# Input
SOURCES += \
  ./src/testunieditorpluginloader.cpp
    
HEADERS += \
  ./inc/testunieditorpluginloader.h
     
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.UID3 =  0xEc0880aa
    TARGET.CAPABILITY = All -TCB -DRM
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    VENDORID =  VID_DEFAULT
    BLD_INF_RULES.prj_exports += "data/TestUnieditorPluginLoaderSample.txt c:/data/TestUnieditorPluginLoaderSample.txt"
    }
	
LIBS += -lunieditorpluginloader \
		-lconvergedmessageutils

packageheader = "$${LITERAL_HASH}{\"TestUnieditorPluginLoader\"},(0xEc0880aa),1,0,0,TYPE=SA"

vendorinfo = \
			"; Localised Vendor name" \
			"%{\"Nokia\"}" \
			"; Unique Vendor name" \
			":\"Nokia\"" 
			
dependencyinfo = \
				"; Default HW/platform dependencies" \
				"[0x101F7961],0,0,0,{\"S60ProductID\"}" \
				"[0x2001E61C],4,6,3,{"Qt"}
				
default_deployment.pkg_prerules =	packageheader \
									vendorinfo \
									dependencyinfo 

#Copy the dependent files
symbian: {
	addFile1.sources = ./data/TestUnieditorPluginLoaderSample.txt
	addFile1.path = C:/data/
	DEPLOYMENT += addFile1
	}
	
symbian:MMP_RULES += SMPSAFE