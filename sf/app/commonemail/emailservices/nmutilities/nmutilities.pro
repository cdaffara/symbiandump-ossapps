#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
TARGET = nmutilities
QT += core
DEFINES += BUILD_UTILITIES_DLL \
           NMCHMAPI_LIBRARY

CONFIG += mobility
MOBILITY += systeminfo \
            contacts

INCLUDEPATH += inc \
               ../../email_plat/contact_history_model_api \
               ../../inc \
               ../../../inc \
               $$APP_LAYER_SYSTEMINCLUDE

DEPENDPATH += src \
              inc \
              ../../email_plat/contact_history_model_api \
              ../../../inc 

MOC_DIR = tmp

HEADERS   += emailutilitiesdef.h \
             emailmailboxinfo_p.h \
             emailmailboxinfo.h \
             emailmru.h \
             nmuieventsnotifier.h \
             nmutilitiescommonheaders.h \
             nmcontacthistorymodel_p.h \
             inc/nmcontacthistorymodelcommon.h \
             inc/nmcontacthistorymodel.h \
             nmuriparser.h
             
SOURCES   += emailmailboxinfo_p.cpp \
             emailmailboxinfo.cpp \
             nmuieventsnotifier.cpp \
             emailmru.cpp \
             nmcontacthistorymodelitem.cpp \
             nmcontacthistorymodel_p.cpp \
             nmcontacthistorymodel.cpp \
             nmuriparser.cpp

symbian*: { 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL \
        -TCB
    TARGET.UID2 = 0x1000008D
    TARGET.UID3 = 0x2002EA97 

    defBlock = \      
      "$${LITERAL_HASH}if defined(MARM)" \
      "DEFFILE  eabi/nmutilities.def" \
      "$${LITERAL_HASH}else" \
      "DEFFILE  bwins/nmutilities.def" \
      "$${LITERAL_HASH}endif"
    
    MMP_RULES += defBlock
    MMP_RULES += SMPSAFE
    
    LIBS += -lcentralrepository 
    LIBS += -ltimezonelocalization 
    LIBS += -lxqsettingsmanager
}


# Build.inf rules  
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             "rom/nmutilities.iby                   $$CORE_APP_LAYER_IBY_EXPORT_PATH(nmutilities.iby)" \
                             "data/gmail.svg                        /epoc32/release/winscw/udeb/z/resource/apps/gmail.svg" \
                             "data/gmail.svg                        /epoc32/release/winscw/urel/z/resource/apps/gmail.svg" \
                             "data/gmail.svg                        /epoc32/data/z/resource/apps/gmail.svg" \
                             "data/microsoft.svg                    /epoc32/release/winscw/udeb/z/resource/apps/microsoft.svg" \
                             "data/microsoft.svg                    /epoc32/release/winscw/urel/z/resource/apps/microsoft.svg" \
                             "data/microsoft.svg                    /epoc32/data/z/resource/apps/microsoft.svg" \
                             "data/ovi.svg                          /epoc32/release/winscw/udeb/z/resource/apps/ovi.svg" \
                             "data/ovi.svg                          /epoc32/release/winscw/urel/z/resource/apps/ovi.svg" \
                             "data/ovi.svg                          /epoc32/data/z/resource/apps/ovi.svg" \
                             "data/yahoo.svg                        /epoc32/release/winscw/udeb/z/resource/apps/yahoo.svg" \
                             "data/yahoo.svg                        /epoc32/release/winscw/urel/z/resource/apps/yahoo.svg" \
                             "data/yahoo.svg                        /epoc32/data/z/resource/apps/yahoo.svg" \
                             "data/aol.svg                          /epoc32/release/winscw/udeb/z/resource/apps/aol.svg" \
                             "data/aol.svg                          /epoc32/release/winscw/urel/z/resource/apps/aol.svg" \
                             "data/aol.svg                          /epoc32/data/z/resource/apps/aol.svg" \
                             "conf/2001E277.txt                     /epoc32/release/winscw/udeb/z/private/10202be9/2001E277.txt" \
                             "conf/2001E277.txt                     /epoc32/release/winscw/urel/z/private/10202be9/2001E277.txt" \
                             "conf/2001E277.txt                     /epoc32/data/z/private/10202be9/2001E277.txt" \
                             "conf/2003EA97.txt                     /epoc32/release/winscw/udeb/z/private/10202be9/2003EA97.txt" \
                             "conf/2003EA97.txt                     /epoc32/release/winscw/urel/z/private/10202be9/2003EA97.txt" \
                             "conf/2003EA97.txt                     /epoc32/data/z/private/10202be9/2003EA97.txt" \
                             "conf/nmutilities.confml               APP_LAYER_CONFML(nmutilities.confml)" \
                             "conf/nmutilities_200255BA.crml        APP_LAYER_CRML(nmutilities_200255BA.crml)" \
                             "inc/emailmru.h                        APP_LAYER_PLATFORM_EXPORT_PATH(emailmru.h)" \
                             "inc/emailutilitiesdef.h               APP_LAYER_PLATFORM_EXPORT_PATH(emailutilitiesdef.h)" \
                             "inc/nmuieventsnotifier.h              APP_LAYER_PLATFORM_EXPORT_PATH(nmuieventsnotifier.h)" \
                             "inc/nmuriparser.h                     APP_LAYER_PLATFORM_EXPORT_PATH(nmuriparser.h)"
