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
# Description: Test Case Project Definition for UnieditorSMSPlugin
#

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = testunieditorsmsplugin

CONFIG += hb
CONFIG += symbian_test
CONFIG += qtestlib

INCLUDEPATH += ./inc
INCLUDEPATH += ../../../../../../../inc
INCLUDEPATH += ../../../../editorgenutils/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TESTUNIEDITORSMSPLUGIN_DLL

SOURCES += \
  ./src/testunieditorsmsplugin.cpp \
  ../../src/unieditorsmsplugin.cpp \
  ../../src/unieditorsmsplugin_p.cpp  
         

# Input
HEADERS += \
  ./inc/testunieditorsmsplugin.h \
  ../../inc/unieditorsmsplugin.h \
  ../../inc/unieditorsmsplugin_p.h 
               
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.UID3 =  0xE2b6e30a
    TARGET.CAPABILITY = All -TCB -DRM
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    VENDORID =  VID_DEFAULT
    BLD_INF_RULES.prj_exports += "data/TestUnieditorSMSPluginSample.txt c:/data/TestUnieditorSMSPluginSample.txt"
    BLD_INF_RULES.prj_exports += "data/TestUnieditorSMSPluginSample.vcf c:/data/TestUnieditorSMSPluginSample.vcf"
    BLD_INF_RULES.prj_exports += "data/TestUnieditorSMSPluginSample.vcs c:/data/TestUnieditorSMSPluginSample.vcs"
    }
   
LIBS += -leuser \
    -lconvergedmessageutils \
    -lMsgMedia \
    -leditorgenutils \
    -lcone \
    -leikcoctl \
    -leikcore \
    -leikdlg \
    -lmsgs \
    -letext \
    -lgsmu \
    -lmmsgenutils \
    -lefsrv \
    -lestor \
    -lsmcm \
    -lCommonEngine \
    -lbafl \
    -lCdlEngine \
    -lFeatMgr \
    -lapmime \
    -lunidatamodelloader \
    -lxqutils \
    -lQtContacts \
    -lxqutils \
    -lapgrfx \
    -lcharconv \
    -lInetProtUtil \
		-lsmildtd \  
		-lxmldom \
  	-lxmlparser \
  	-lcone \
  	-lQtCore \
  	-letel \
  	-letelmm \
  	-lcentralrepository

packageheader = "$${LITERAL_HASH}{\"TestUnieditorSMSPlugin\"},(0xE2b6e30a),1,0,0,TYPE=SA"

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

#Copy the dependent DLL
symbian: {
	addFile1.sources = ./data/TestUnieditorSMSPluginSample.txt
	addFile1.path = C:/data/
	DEPLOYMENT += addFile1
	
	addFile2.sources = ./data/TestUnieditorSMSPluginSample.vcf
	addFile2.path = C:/data/
	DEPLOYMENT += addFile2
	
	addFile3.sources = ./data/TestUnieditorSMSPluginSample.vcs
	addFile3.path = C:/data/
	DEPLOYMENT += addFile3
	}
	
symbian:MMP_RULES += SMPSAFE
