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
# Description:  FTU default runtime provider project file
#

TEMPLATE = lib

symbian {
    CONFIG(debug, debug|release) {
        DESTDIR = debug
    } else {  
        DESTDIR = release
    }
}

win32 {
    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }
    DESTDIR = $$PWD/../../../bin/$$SUBDIRPART/fturesources/plugins/runtimeproviders
    LIBS += -L$$PWD/../../../bin/debug
}

LIBS +=  -lfturuntimeservices
LIBS +=  -lftustateprovider
    
CONFIG += plugin debug_and_release

CONFIG += hb

CONFIG += hb mobility
MOBILITY = serviceframework

QT += xml sql

DEFINES += FTUDEFAULTRUNTIMEPROVIDER_LIB
#DEFINES += LOGGING

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH +=   ./inc \
                ./src

INCLUDEPATH += ./inc \
               ../../inc \

symbian {
    TARGET.UID3 = 0x20026F97
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    appkey:DEFINES += S60APP_KEY
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    include(ftudefaultruntimeprovider_installs_symbian.pri)

     #Add default symbian libs required by the application (redundant/duplicate entries to be removed)
     LIBS += -lcone -leikcore -lmediaclientaudio -leikcoctl -leiksrv -lapparc -lavkon
     LIBS += -lefsrv -lcharconv -lws32 -lhal -lgdi -lapgrfx

}

win32 {
include(ftudefaultruntimeprovider_installs_win32.pri)
}

symbian:unix:include(ftudefaultruntimeprovider_installs_unix.pri)

include(ftudefaultruntimeprovider.pri)

symbian: plugin{
    BLD_INF_RULES.prj_exports += "resource/$${TARGET}.s60xml z:$$qtplugins.path/$${TARGET}.xml"
    
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)"

}


symbian:MMP_RULES += SMPSAFE
