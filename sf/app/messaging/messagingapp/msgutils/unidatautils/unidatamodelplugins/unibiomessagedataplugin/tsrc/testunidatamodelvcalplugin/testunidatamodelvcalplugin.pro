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
TARGET = testunidatamodelvcalplugin

CONFIG += hb
CONFIG += symbian_test
CONFIG += qtestlib

INCLUDEPATH += .
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += ../../../../../../../inc
INCLUDEPATH += ../../../../../../../../../../epoc32/include/platform/mw
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
	src/testunidatamodelvcalplugin.cpp

				
# Input
HEADERS += \
	inc/testunidatamodelvcalplugin.h
	
	   	   			 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += "vcal1.txt /epoc32/winscw/c/test/vcal1.txt"
    BLD_INF_RULES.prj_exports += "BioMtm.rsc /epoc32/winscw/c/resource/messaging/mtm/BioMtm.rsc"
    BLD_INF_RULES.prj_exports += "data/TestUniDataModelVCalPlugin.cfg c:/TestUniDataModelVCalPlugin.cfg"
    BLD_INF_RULES.prj_exports += "data/TestUniDataModelVCalPlugin.pl c:/TestUniDataModelVCalPlugin.pl"
    		}


packageheader = "$${LITERAL_HASH}{\"testunidatamodelvcalplugin\"},(0xE2815e10),1,0,0,TYPE=SA"

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
	addCfg.sources = ./data/TestUniDataModelVCalPlugin.cfg
	addCfg.path = C:/
	DEPLOYMENT += addCfg

	addPl.sources = ./data/TestUniDataModelVCalPlugin.pl
	addPl.path = C:/
	DEPLOYMENT += addPl

	addP2.sources = ./vcal1.txt
	addP2.path = C:/test
	DEPLOYMENT += addP2

	}


 LIBS += -leuser \
	-lconvergedmessageutils\
	-lmsgs \
	-lsmcm \
	-lgsmu \
	-letext \
	-lmsgs \
	-lunidatamodelloader\
	-lQtCore \
	-lbioc \
	-lefsrv \
	-lbiodb \
     -lxqutils \
	-lbifu
	



symbian:MMP_RULES += SMPSAFE
