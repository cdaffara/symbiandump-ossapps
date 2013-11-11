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
# Description: Project file for test_mms_plugin
#

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = testmmsplugin

INCLUDEPATH += ./inc
INCLUDEPATH += ../../../../../../../../inc
INCLUDEPATH += ../../../../../../unidatautils/unidatamodels/inc
INCLUDEPATH += ../../../../../editorgenutils/inc
INCLUDEPATH += ../../../../../../../../inc
INCLUDEPATH += ../../../../../../../../mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../../../../../../../mmsengine/inc
INCLUDEPATH += ../../../../../../unidatautils/unidatamodel/inc
INCLUDEPATH += ../mmstestbed/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

CONFIG += hb
CONFIG += symbian_test
CONFIG += qtestlib

SOURCES += \
  ./src/testmmsplugin.cpp \
  ../../../src/unieditormmsplugin.cpp \
  ../../../src/unieditormmsplugin_p.cpp  
         
# Input
HEADERS += \
  ./inc/testmmsplugin.h \
  ../../../inc/unieditormmsplugin.h \
  ../../../inc/unieditormmsplugin_p.h 
               
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.UID3 =  0xE274bf60
#    TARGET.CAPABILITY = All -TCB -DRM
    TARGET.CAPABILITY = CAP_APPLICATION
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    VENDORID =  VID_DEFAULT
    BLD_INF_RULES.prj_exports += "data/TestMMSPluginSample.txt c:/data/TestMMSPluginSample.txt"
	BLD_INF_RULES.prj_exports += "data/TestMMSPluginSmileyFace.gif c:/data/TestMMSPluginSmileyFace.gif"	
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
    -lapgrfx \
    -lcharconv \
    -lInetProtUtil \
		-lsmildtd \  
		-lxmldom \
  	-lxmlparser \
  	-lcone \
  	-lQtCore \
  	-letel \
  	-lcommdb \
  	-lcommsdat \
  	-letelmm \
  	-lunidatamodelloader \
  	-lunidatamodel \
  	-lavkon \
  	-leikcoctl \
  	-leikctl \
  	-lform \
  	-luiklaf\ 
  	-lmmstestbed \
	-lmmsmessage \ 
	-lmmsserversettings \
	-lxqutils \
	-lQtContacts
	
packageheader = "$${LITERAL_HASH}{\"TestMMSPlugin\"},(0xE274bf60),1,0,0,TYPE=SA"

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
	addDll.sources = mmstestbed.dll
	addDll.path = /sys/bin
	DEPLOYMENT += addDll
	
	addFile1.sources = ./data/TestMMSPluginSample.txt
	addFile1.path = C:/data/
	DEPLOYMENT += addFile1
	
	addFile2.sources = ./data/TestMMSPluginSmileyFace.gif
	addFile2.path = C:/data/
	DEPLOYMENT += addFile2
	}
	
symbian:MMP_RULES += SMPSAFE
