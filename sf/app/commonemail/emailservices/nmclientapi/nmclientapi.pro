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
#

CONFIG += debug
TEMPLATE = lib
TARGET = nmailclientapi
QT += core

# BUILD_DLL
DEFINES += BUILD_EMAIL_API_DLL

INCLUDEPATH +=  inc \
                ../../email_plat/nmail_client_api/inc \
                ../../email_plat/nmail_settings_api/inc \
                ../../inc

DEPENDPATH +=   ../../email_plat/nmail_client_api/inc \
                ../../email_plat/nmail_settings_api/inc \
                $$APP_LAYER_SYSTEMINCLUDE 

MOC_DIR = tmp

# public headers 
HEADERS += nmapicommon.h \
           nmapidef.h \
           nmapiemailaddress.h \
           nmapiemailservice.h \
           nmapienvelopelisting.h \
           nmapieventnotifier.h \
           nmapifolder.h \
           nmapifolderlisting.h \
           nmapimailbox.h \
           nmapimailboxlisting.h \
           nmapimessagebody.h \
           nmapimessageenvelope.h \
           nmapimessagetask.h \
           nmapimessagemanager.h \
           nmapisettingsmanager.h \
           nmapimailboxsettings.h \
           nmapioperation.h \
           nmapimessagesearch.h \
           nmapimessage.h \
           nmapimessagecontent.h \
           nmapitextcontent.h \
           nmapiattachment.h

# private headers 
HEADERS += inc/nmapidatapluginfactory.h \
           inc/nmapiemailaddress_p.h \
           inc/nmapiengine.h \
           inc/nmapienvelopelisting_p.h \
           inc/nmapieventnotifier_p.h \
           inc/nmapifolder_p.h \
           inc/nmapifolderlisting_p.h \
           inc/nmapimailbox_p.h \
           inc/nmapimailboxlisting_p.h \
           inc/nmapimailboxsettings_p.h \
           inc/nmapimailboxsettingsdata_p.h \
           inc/nmapimessagebody_p.h \
           inc/nmapimessageenvelope_p.h \
           inc/nmapimessagemanager_p.h \
           inc/nmapisettingsmanager_p.h \
           inc/nmapipopimapsettingsmanager.h \
           inc/nmapiprivateclasses.h \
           inc/nmapitypesconverter.h \
           inc/nmapiheaders.h \
           inc/nmapimessagesearch_p.h \
           inc/nmapimessage_p.h \
           inc/nmapimessagecontent_p.h \
           inc/nmapitextcontent_p.h \
           inc/nmapiattachment_p.h \
           inc/nmapioperationcreatedraftmessage.h \
           inc/nmapioperationsendmessage.h
	   
SOURCES += src/nmapidatapluginfactory.cpp \
           src/nmapiemailaddress.cpp \
           src/nmapiemailservice.cpp \
           src/nmapiengine.cpp \
           src/nmapienvelopelisting_p.cpp \
           src/nmapienvelopelisting.cpp \ 
           src/nmapieventnotifier_p.cpp \
           src/nmapieventnotifier.cpp \
           src/nmapifolder.cpp \
           src/nmapifolderlisting_p.cpp \
           src/nmapifolderlisting.cpp \
           src/nmapimailbox.cpp \
           src/nmapimailboxlisting_p.cpp \
           src/nmapimailboxlisting.cpp \
           src/nmapimessagetask.cpp \
           src/nmapimailboxsettingsdata_p.cpp \
           src/nmapimailboxsettingsdata.cpp \
           src/nmapimailboxsettings_p.cpp \
           src/nmapimailboxsettings.cpp \
           src/nmapimessagebody.cpp \
           src/nmapimessageenvelope.cpp \
           src/nmapipopimapsettingsmanager.cpp \
           src/nmapimessagemanager_p.cpp \
           src/nmapimessagemanager.cpp \
           src/nmapisettingsmanager_p.cpp \
           src/nmapisettingsmanager.cpp \
           src/nmapitypesconverter.cpp \
           src/nmapioperation.cpp \
           src/nmapimessagesearch.cpp \
           src/nmapimessagesearch_p.cpp \
           src/nmapimessage.cpp \
           src/nmapimessage_p.cpp \
           src/nmapimessagecontent.cpp \
           src/nmapimessagecontent_p.cpp \
           src/nmapitextcontent.cpp \
           src/nmapitextcontent_p.cpp \
           src/nmapiattachment.cpp \
           src/nmapiattachment_p.cpp \
           src/nmapioperationcreatedraftmessage.cpp \
           src/nmapioperationsendmessage.cpp




symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB 
    TARGET.CAPABILITY += CAP_ECOM_PLUGIN

    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002C366

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    MMP_RULES += "SYSTEMINCLUDE /epoc32/include/ecom" 

     LIBS += -lnmailbase \
     -limcm \
     -lxqsettingsmanager \
     -lxqutils \
     -lnmailuiengine \
     -lFSFWCommonLib \
     -lECom \
     -lxqserviceutil \
     -lefsrv 

    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmailclientapi.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmailclientapi.def" \
      "$${LITERAL_HASH}endif"

    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE

     BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/nmailclientapi.iby CORE_APP_LAYER_IBY_EXPORT_PATH(nmailclientapi.iby)"

}

