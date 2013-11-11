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
# Description:  SEARCH default runtime provider project file
#

TEMPLATE = lib

DEFINES += BUILD_SEARCHRUNTIMEPROVIDER

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
    DESTDIR = $$PWD/../../../bin/$$SUBDIRPART/searchresources/plugins/runtimeproviders    
    LIBS += -L$$PWD/../../../bin/debug
}

LIBS +=  -lsearchstateprovider
    


CONFIG += hb


QT += xml sql



coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH +=   ./inc \
                ./src 
                
INCLUDEPATH += ./inc \
               ../../inc  \
                ../../stateproviders/searchstateprovider/inc 
               

symbian {
    TARGET.UID3 = 0x2002C37D
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    appkey:DEFINES += S60APP_KEY
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
    defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" 	\
	"DEFFILE  ../eabi/" 				\
    "$${LITERAL_HASH}else" 				\
    "DEFFILE  ../bwins/" 				\
    "$${LITERAL_HASH}endif"
	
	MMP_RULES += defBlock
    #include(searchruntimeprovider_installs_symbian.pri)
   # BLD_INF_RULES.prj_exports += "./inc/searchruntimeprovider.h |../../inc/" \
    #                             "./inc/searchruntime.h |../../inc/"
}

win32 {
include(searchruntimeprovider_installs_win32.pri)
}

symbian:unix:include(searchruntimeprovider_installs_unix.pri)

include(searchruntimeprovider.pri)
