
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

CONFIG += hb
CONFIG += symbian_test

TEMPLATE = app
TARGET = testmmsplugin

INCLUDEPATH += inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../../../../../../inc
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/inc
INCLUDEPATH += ../../../../../msgutils/unidatautils/unidatamodel/inc
INCLUDEPATH += ../../../../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += ../../../../../../../../mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../../../../../../../mmsengine/inc
INCLUDEPATH += ../mmstestbed/inc
INCLUDEPATH +=
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
  src/testmmsplugin.cpp \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/src/unieditormmsplugin.cpp \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/src/unieditormmsplugin_p.cpp  
      

# Input
HEADERS += \
  inc/testmmsplugin.h \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/inc/unieditormmsplugin.h \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/inc/unieditormmsplugin_p.h 
               
   
SYMBIAN_PLATFORMS = WINSCW ARMV5

symbian {
    TARGET.UID3 =  0xE7232a7c
#    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.CAPABILITY = CAP_APPLICATION
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
    VENDORID =  VID_DEFAULT
    BLD_INF_RULES.prj_exports += "data/testmmsplugin.cfg c:/testmmsplugin.cfg"
    BLD_INF_RULES.prj_exports += "data/testmmsplugin.pl  c:/testmmsplugin.pl"
    BLD_INF_RULES.prj_exports += "data/SmileyFace.gif    c:/SmileyFace.gif"
    BLD_INF_RULES.prj_exports += "data/sample.txt        c:/sample.txt"

	}

 LIBS += -leuser \
    -lcsserverclientapi \ 
    -lcsutils \
    -lconvergedmessageutils \
    -lMsgMedia \
    -leikctl \
    -leditorgenutils \
    -lcone \
    -leikcoctl \
    -leikcore \
    -leikdlg \
    -lmsgs \
    -letext \
    -lmmsserversettings \
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
    -lmmsmessage \  
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
    -lxqutils\ 
    -lmmstestbed\
   -lmsginit \
        -lmsgs \
        -letext \
        -lsmcm \
        -lmmsmessage \
        -lmmsserversettings \
        -lmmscli \
        -lxqservice \
        -lQtContacts \
        -lQtVersit \
        -lunidatamodelloader \
	-lxqsettingsmanager \
        -apgrfx.lib \
        -lxqutils \
        -lws32 \
        -lapgrfx \
	-lxqsystemtoneservice

  	

packageheader = "$${LITERAL_HASH}{\"QTestMsgNotifier\"},(0xE7232a7c),1,0,0,TYPE=SA"

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
	addCfg.sources = ./data/testmmsplugin.cfg
	addCfg.path = C:/
	DEPLOYMENT += addCfg

	addPl.sources = ./data/testmmsplugin.pl
	addPl.path = C:/
	DEPLOYMENT += addPl

	addFiles.sources = mmstestbed.dll
	addFiles.path = /sys/bin
	DEPLOYMENT += addFiles

      addP2.sources = ./data/SmileyFace.gif
	addP2.path = C:/
	DEPLOYMENT += addP2

	addP3.sources = ./data/sample.txt
	addP3.path = C:/
	DEPLOYMENT += addP3	


}

symbian:MMP_RULES += SMPSAFE
