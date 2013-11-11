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

TEMPLATE = lib
TARGET = phoneengine

CONFIG += mobility
MOBILITY = systeminfo
QT -= gui
QT += network

symbian {
    TARGET.UID3 = 0x1000A86C
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
    VERSION = 10.0
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE  \
                   $$OS_LAYER_GLIB_SYSTEMINCLUDE
    INCLUDEPATH += ../../inc \
                   ../inc \
                   ../phoneservices/inc \
                   /epoc32/include/mw/QtGui
    
    engineDefFiles = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "DEFFILE bwins/phoneengine.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE eabi/phoneengine.def" \
        "$${LITERAL_HASH}endif"
    
    MMP_RULES += engineDefFiles

    LIBS += -lcce \
            -lAccClient \
            -lapgrfx \
            -lapparc \
            -laudiohandling \
            -lcallhandling \
            -lcentralrepository \
            -lcommonEngine \
            -lcontacthandling \
            -lefsrv \
            -letel \
            -letelmm \
            -leuser \
            -lFeatMgr \
            -lflogger \
            -lloghandling \
            -lphoneparser \
            -lphonesettings \
            -lsecui \
            -lSysUtil \
            -lws32 \
            -lPlatformEnv \
            -lPhoneClient \
            -lprofileeng \
            -lbafl \
            -lcallprovider \
            -lpeservicehandling \
            -lphoneservices \
            -lengineinfo \
            -lmediatorclient \
            -lestor \
            -lxqservice \
            -lxqserviceutil \
            -llibglib \
            -lpsetwrapper \
            -lpsuinotes
            
    TARGET.EPOCALLOWDLLDATA =1
    
}

HEADERS += ./inc/cpeaccessorymodemonitor.h
HEADERS += ./inc/cpeactivestarter.h
HEADERS += ./inc/cpeaudiooutputpreferencemonitor.h
HEADERS += ./inc/cpecalldurationdisplaysettingmonitor.h
HEADERS += ./inc/cpecenrepmonitor.h
HEADERS += ./inc/cpeclientcallrequestmonitor.h
HEADERS += ./inc/cpeclientcommandhandlermonitor.h
HEADERS += ./inc/cpeclientemergencycallmonitor.h
HEADERS += ./inc/cpeclientservices.h
HEADERS += ./inc/cpedevicemodehandler.h
HEADERS += ./inc/cpeexternaldatahandler.h
HEADERS += ./inc/cpemanualcallcontrolhandler.h
HEADERS += ./inc/cpemessagehandler.h
HEADERS += ./inc/cpemessagehandler.inl
HEADERS += ./inc/cpenetworkregistrationstatusmonitor.h
HEADERS += ./inc/cpeparseremergencynumberhandler.h
HEADERS += ./inc/cpeparsermanufacturerhandler.h
HEADERS += ./inc/cpeparsermischandler.h
HEADERS += ./inc/cpeparserphonenumberhandler.h
HEADERS += ./inc/cpeparsersimcontrolhandler.h
HEADERS += ./inc/cpeparsersscallhandler.h
HEADERS += ./inc/cpeparsersshandler.h
HEADERS += ./inc/cpeparservoipnumberhandler.h
HEADERS += ./inc/cpepcnparserprocedurehandler.h
HEADERS += ./inc/cpephonemodel.h
HEADERS += ./inc/cpephonemodelif.h
HEADERS += ./inc/cpeprofilesettingmonitor.h
HEADERS += ./inc/cpepubsubmonitor.h
HEADERS += ./inc/cpesettingscommand.h
HEADERS += ./inc/cpesimstatemonitor.h
HEADERS += ./inc/cpetimer.h
HEADERS += ./inc/mpeactivestarter.h
HEADERS += ./inc/mpecallcontrolif.h
HEADERS += ./inc/mpecallsettersif.h
HEADERS += ./inc/mpeclientcallrequestmonitor.h
HEADERS += ./inc/mpeexternaldatahandler.h
HEADERS += ./inc/mpephonemodelinternal.h
HEADERS += ./inc/mphonedevicemodeobserver.h
HEADERS += ./inc/pepanic.pan
HEADERS += ./inc/pevirtualengine.h
HEADERS += ./inc/talogger.h
HEADERS += ./inc/telephonyvariant.hrh
HEADERS += ./inc/telinternalcrkeys.h
HEADERS += ./inc/telinternalpskeys.h
HEADERS += ./inc/telinternalpstypes.h
HEADERS += ./inc/penetworkhandler.h

SOURCES += ./src/cpeaccessorymodemonitor.cpp
SOURCES += ./src/cpeactivestarter.cpp
SOURCES += ./src/cpeaudiooutputpreferencemonitor.cpp
SOURCES += ./src/cpecalldurationdisplaysettingmonitor.cpp
SOURCES += ./src/cpecenrepmonitor.cpp
SOURCES += ./src/cpeclientcallrequestmonitor.cpp
SOURCES += ./src/cpeclientcommandhandlermonitor.cpp
SOURCES += ./src/cpeclientemergencycallmonitor.cpp
SOURCES += ./src/cpeclientservices.cpp
SOURCES += ./src/cpedevicemodehandler.cpp
SOURCES += ./src/cpeexternaldatahandler.cpp
SOURCES += ./src/cpemessagehandler.cpp
SOURCES += ./src/cpenetworkregistrationstatusmonitor.cpp
SOURCES += ./src/cpeprofilesettingmonitor.cpp
SOURCES += ./src/cpepubsubmonitor.cpp
SOURCES += ./src/cpetimer.cpp
SOURCES += ./src/pepanic.cpp
SOURCES += ./src/cpemanualcallcontrolhandler.cpp
SOURCES += ./src/cpeparsermanufacturerhandler.cpp
SOURCES += ./src/cpeparsermischandler.cpp
SOURCES += ./src/cpeparserphonenumberhandler.cpp
SOURCES += ./src/cpeparsersimcontrolhandler.cpp
SOURCES += ./src/cpeparsersscallhandler.cpp
SOURCES += ./src/cpeparsersshandler.cpp
SOURCES += ./src/cpephonemodel.cpp
SOURCES += ./src/cpephonemodelif.cpp
SOURCES += ./src/cpesimstatemonitor.cpp
SOURCES += ./src/cpepcnparserprocedurehandler.cpp
SOURCES += ./src/cpeparseremergencynumberhandler.cpp
SOURCES += ./src/cpeparservoipnumberhandler.cpp
SOURCES += ./src/phoneengineproxy.cpp
SOURCES += ./src/cperemotepartyinfomediator.cpp 
SOURCES += ./src/cperemotepartyinfoproxy.cpp
SOURCES += ./src/tperemotepartyinfomediatorupdater.cpp
SOURCES += ./src/penetworkhandler.cpp

include(api_headers.pri)
    
headers.sources = $$PHONEENGINE_API_HEADERS
headers.path = |../../inc
# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$headers.path/$$basename(header)"       

BLD_INF_RULES.prj_exports += \
 "$${LITERAL_HASH}include <platform_paths.hrh>" \
 "./rom/phonengengine.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(phonengengine.iby)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony.confml APP_LAYER_CONFML(s60telephony.confml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_japan.confml CONFML_EXPORT_PATH(s60telephony_japan.confml,japan)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_101f87e3.crml APP_LAYER_CRML(s60telephony_101f87e3.crml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_102828b1.crml APP_LAYER_CRML(s60telephony_102828b1.crml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_102828b2.crml APP_LAYER_CRML(s60telephony_102828b2.crml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_102828b8.crml APP_LAYER_CRML(s60telephony_102828b8.crml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_102828f8.crml APP_LAYER_CRML(s60telephony_102828f8.crml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_102830b6.crml APP_LAYER_CRML(s60telephony_102830b6.crml)"
BLD_INF_RULES.prj_exports += "./conf/s60telephony_2001b2e6.crml APP_LAYER_CRML(s60telephony_2001b2e6.crml)"

#Backup registration files
BLD_INF_RULES.prj_exports += "./data/backup_registration.xml /epoc32/data/z/private/1000a86c/backup_registration.xml"
BLD_INF_RULES.prj_exports += "./data/backup_registration.xml /epoc32/release/winscw/udeb/z/private/1000a86c/backup_registration.xml"
BLD_INF_RULES.prj_exports += "./data/backup_registration.xml /epoc32/release/winscw/urel/z/private/1000a86c/backup_registration.xml"

#Stub sis
BLD_INF_RULES.prj_exports += "./rom/phonemodel_stub.sis /epoc32/data/z/system/install/phonemodel_stub.sis"

symbian:MMP_RULES += SMPSAFE
