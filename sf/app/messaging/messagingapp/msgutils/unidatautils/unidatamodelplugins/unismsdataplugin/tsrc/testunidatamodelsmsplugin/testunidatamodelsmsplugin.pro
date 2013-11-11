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
QT -= gui

TEMPLATE = app
TARGET = test-unidatamodel-sms-plugin


CONFIG += hb
CONFIG += symbian_test

INCLUDEPATH += inc
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += ../../../../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
	src/testunidatamodelsmsplugin.cpp

				 

# Input
HEADERS += \
	inc/testunidatamodelsmsplugin.h
	
	   	   			 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = All -TCB -DRM
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    VENDORID =  VID_DEFAULT
    }
	
defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/test_unidatamodel_sms_plugin.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/test_unidatamodel_sms_plugin.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock
		

packageheader = "$${LITERAL_HASH}{\"QTestLibCVsClientServerTest\"},(0xEa1ebe60),1,0,0,TYPE=SA"

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



 LIBS += -leuser \
	-lconvergedmessageutils\
	-lxqutils \
	-lmsgs \
	-lsmcm \
	-lgsmu \
	-letext \
	-lmsgs \
	-lunidatamodelloader\
	-lQtCore
	


symbian:MMP_RULES += SMPSAFE
