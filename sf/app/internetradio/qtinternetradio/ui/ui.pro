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

#include <platform_paths.hrh>
include(../common.pri)

TEMPLATE = app
TARGET = internet_radio_10_1
TRANSLATIONS += internet_radio_10_1.ts
TARGET.CAPABILITY += NetworkServices  AllFiles  ReadDeviceData WriteDeviceData ReadUserData WriteUserData

MOC_DIR = moc
DEPENDPATH = src

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
INCLUDEPATH += $${OS_LAYER_PUBLIC_EXPORT_PATH(http)}
INCLUDEPATH += $${MW_LAYER_PUBLIC_EXPORT_PATH(http)}

#the following is for splash screen in phone
BLD_INF_RULES.prj_exports += "resources/splashscreen/internet_radio_normal.splashml        /epoc32/data/z/resource/hb/splashml/internet_radio_normal.splashml" \
                             "resources/splashscreen/internet_radio_normal.docml           /epoc32/data/z/resource/hb/splashml/internet_radio_normal.docml" \
                             "resources/splashscreen/internet_radio_nowplaying.splashml    /epoc32/data/z/resource/hb/splashml/internet_radio_nowplaying.splashml" \
                             "resources/splashscreen/internet_radio_nowplaying.docml       /epoc32/data/z/resource/hb/splashml/internet_radio_nowplaying.docml" \
                             "resources/splashscreen/internet_radio_termscons.splashml     /epoc32/data/z/resource/hb/splashml/internet_radio_termscons.splashml" \
                             "resources/splashscreen/internet_radio_termscons.docml        /epoc32/data/z/resource/hb/splashml/internet_radio_termscons.docml"

#the following is for splash screen in winscw_udeb
BLD_INF_RULES.prj_exports += "resources/splashscreen/internet_radio_normal.splashml       /epoc32/release/winscw/udeb/z/resource/hb/splashml/internet_radio_normal.splashml" \
                             "resources/splashscreen/internet_radio_normal.docml          /epoc32/release/winscw/udeb/z/resource/hb/splashml/internet_radio_normal.docml" \
                             "resources/splashscreen/internet_radio_nowplaying.splashml   /epoc32/release/winscw/udeb/z/resource/hb/splashml/internet_radio_nowplaying.splashml" \
                             "resources/splashscreen/internet_radio_nowplaying.docml      /epoc32/release/winscw/udeb/z/resource/hb/splashml/internet_radio_nowplaying.docml" \
                             "resources/splashscreen/internet_radio_termscons.splashml    /epoc32/release/winscw/udeb/z/resource/hb/splashml/internet_radio_termscons.splashml" \
                             "resources/splashscreen/internet_radio_termscons.docml       /epoc32/release/winscw/udeb/z/resource/hb/splashml/internet_radio_termscons.docml" 

#the following is for splash screen in winscw_urel
BLD_INF_RULES.prj_exports += "resources/splashscreen/internet_radio_normal.splashml       /epoc32/release/winscw/urel/z/resource/hb/splashml/internet_radio_normal.splashml" \
                             "resources/splashscreen/internet_radio_normal.docml          /epoc32/release/winscw/urel/z/resource/hb/splashml/internet_radio_normal.docml" \
                             "resources/splashscreen/internet_radio_nowplaying.splashml   /epoc32/release/winscw/urel/z/resource/hb/splashml/internet_radio_nowplaying.splashml" \
                             "resources/splashscreen/internet_radio_nowplaying.docml      /epoc32/release/winscw/urel/z/resource/hb/splashml/internet_radio_nowplaying.docml" \                         
                             "resources/splashscreen/internet_radio_termscons.splashml    /epoc32/release/winscw/urel/z/resource/hb/splashml/internet_radio_termscons.splashml" \
                             "resources/splashscreen/internet_radio_termscons.docml       /epoc32/release/winscw/urel/z/resource/hb/splashml/internet_radio_termscons.docml"
                             


INCLUDEPATH += inc \
               ../irqisdsclient/inc \
               ../irqmediaplayer/inc \
               ../irqcommon/inc \
               ../irqstatisticsreporter/inc \
               ../irqfavoritesdb/inc \
               ../irqnetworkcontroller/inc \                
               ../irqsettings/inc    \                
               ../irqlogger/inc \       
               ../irdb/inc \               
               ../irqsystemeventhandler/inc

CONFIG += hb service
QT += sql

SERVICE.FILE = service_conf.xml

LIBS += -lirqnetworkcontroller \         
        -lirqisdsclient \
        -lirqmediaplayer \
        -lirqutility \
        -lirqfavorites \      
        -lirqsettings \	    
        -lremconcoreapi \
        -lremconinterfacebase \
        -lirqstatisticsreporter \         
        -lirqlogger \
        -lshareui \
        -lirqsystemeventhandler \
        -lxqsettingsmanager \
        -lxqservice \
        -lxqserviceutil \
        -lirdb \
        -lhgcacheproxymodel
        	
symbian{
  TARGET.UID3 = 0x2002FFAC
  TARGET.EPOCHEAPSIZE = 0x020000 0x2000000
  TARGET.VID = VID_DEFAULT
  SKINICON = qtg_large_internet_radio
  datatype_list = "datatype_list={DATATYPE{priority=EDataTypePriorityHigh;type=\"audio/x-scpls\";}};"
  RSS_RULES += datatype_list 
  MMP_RULES += SMPSAFE
}

#input
#header files list
HEADERS += inc/irapplication.h \
           inc/irviewmanager.h \
           inc/irbaseview.h \
           inc/irmainview.h \
           inc/irmainmodel.h \
           inc/ircategoryview.h \      
           inc/irstationsview.h \  
           inc/irnowplayingview.h \
           inc/irplaycontroller.h \
           inc/ircategorymodel.h \
           inc/irchannelmodel.h \
           inc/irchanneldataprovider.h \
           inc/irabstractlistviewbase.h \
           inc/irfavoritesview.h \
           inc/irfavoritesmodel.h \
           inc/irhistoryview.h \
           inc/irsonghistoryview.h \
           inc/irsettingsview.h \
           inc/irlastplayedstationinfo.h \
           inc/iropenwebaddressview.h \
           inc/irhistorymodel.h \
           inc/irsonghistorymodel.h \            
           inc/irsearchchannelsview.h \
           inc/irdocumentloader.h \           
           inc/irmediakeyobserver.h \
           inc/irplsmodel.h \
           inc/irplsview.h \
           inc/irmemorycollector.h \
           inc/irstationshare.h \
           inc/irstationdetailsview.h \
           inc/irtermsconsview.h \
           inc/ircontrolservice.h \
           inc/irmonitorservice.h \           
           inc/irplaylist.h \
           inc/irsearchresultdb.h \
           inc/irfileviewservice.h \
           inc/irplayingbanner.h \
           inc/irhbapplication.h \
           inc/irlogoprovider.h \
	   inc/irqsonghistoryinfo.h
          
           
#source files list
SOURCES += main.cpp  \
           irapplication.cpp \
           irviewmanager.cpp \
           irbaseview.cpp \
           irmainview.cpp \
           irmainmodel.cpp \
           ircategoryview.cpp \
           irstationsview.cpp \
           irnowplayingview.cpp \
           irplaycontroller.cpp \
           ircategorymodel.cpp \
           irchannelmodel.cpp \
           irchanneldataprovider.cpp \
           irabstractlistviewbase.cpp \
           irfavoritesview.cpp \
           irfavoritesmodel.cpp \
           irhistoryview.cpp \
           irsonghistoryview.cpp \
           irsettingsview.cpp \
           irlastplayedstationinfo.cpp \
           iropenwebaddressview.cpp \
           irhistorymodel.cpp \
           irsonghistorymodel.cpp \                   
           irsearchchannelsview.cpp \
           irdocumentloader.cpp \            
           irmediakeyobserver.cpp \
           irplsmodel.cpp \
           irplsview.cpp \
           irmemorycollector.cpp \
           irstationshare.cpp \
           irstationdetailsview.cpp \
           irtermsconsview.cpp \
           ircontrolservice.cpp \
           irmonitorservice.cpp \
           irplaylist.cpp \
           irsearchresultdb.cpp \
           irfileviewservice.cpp \
           irplayingbanner.cpp \
           irhbapplication.cpp \
           irlogoprovider.cpp \
           irqsonghistoryinfo.cpp
            

#docml      
DOCML += resources/layout/abstractlistviewbase.docml \
         resources/layout/nowplayingview.docml \
         resources/layout/openwebaddressview.docml \
         resources/layout/searchchannelsview.docml \
         resources/layout/stationdetailsview.docml \
         resources/layout/termsconsview.docml
               	   
#resource
RESOURCES += resources/irui.qrc

SYMBIAN_PLATFORMS = WINSCW ARMV5

!exists( $(RVCT22BIN) ) {       
        !exists( $(RVCT31BIN) ) {
           exists( $(RVCT40BIN) ) {       
               QMAKE_CFLAGS.ARMCC += --import_all_vtbl   
           }           
        }
 }
 
 
 
 