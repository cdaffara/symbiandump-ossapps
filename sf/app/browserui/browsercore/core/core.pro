#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, version 2.1 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, 
# see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
#
# Description:
#


QT += webkit

TEMPLATE = lib
TARGET = BrowserCore
DEFINES += BUILDING_BROWSER_CORE
DEFINES += BUILDING_BWF_CORE

NETWORK_DEBUG: DEFINES+=NETWORK_DEBUG

################################################################################

# establish relative root dir
ROOT_DIR = $$PWD/../..

QT += webkit
include($$ROOT_DIR/flags.pri)
include($$ROOT_DIR/browserui.pri)

contains(DEFINES, ENABLE_PERF_TRACE) {
message("core.pro: Tracing is ON")
LIBS += -lbrperftrace
INCLUDEPATH += $$ROOT_DIR/internal/tests/perfTracing
}

isEmpty(WRT_OUTPUT_DIR) {
    symbian {
        CONFIG(release, debug|release):WRT_OUTPUT_DIR=$$PWD/../../WrtBuild/Release
        CONFIG(debug, debug|release):WRT_OUTPUT_DIR=$$PWD/../../WrtBuild/Debug
    } 
    else {
        CONFIG(release, debug|release):WRT_OUTPUT_DIR=$$ROOT_DIR/../../../WrtBuild/Release
        CONFIG(debug, debug|release):WRT_OUTPUT_DIR=$$ROOT_DIR/../../../WrtBuild/Debug
    }
}

LIBS += -lBedrockProvisioning -lbrowsercontentdll  
LIBS += -lbookmarksapi

contains(br_orbit_ui, yes) {
    LIBS += -lshareui -lxqservice
    DEFINES += ORBIT_UI
}

contains(br_layout, tenone) {
    DEFINES +=BROWSER_LAYOUT_TENONE
}

RESOURCES += $$PWD/../browsercore.qrc

symbian: {
    SYMBIAN_PUB_SDK = $$(SYMBIAN_PUB_SDK)
    isEmpty(SYMBIAN_PUB_SDK) {
        LIBS += -lsisregistryclient -letelmm
    } else {
        DEFINES += SYMBIAN_PUB_SDK
    }
    #LIBS += -llibpthread -letel -lsysutil -lWrtTelService -lsendui -letext -lcommonengine -lcone -lefsrv 
    LIBS += -llibpthread -letel -lsysutil -lsendui -letext -lcommonengine -lcommonui -lcone -lefsrv -lServiceHandler -lapmime -lapparc

    isEmpty(SYMBIAN_PUB_SDK) {
    LIBS +=  \
        -laiwdialdata
		}

    AIWResource = \
        "START RESOURCE WrtTelService.rss" \
        "HEADER" \
        "TARGETPATH resource/apps" \
        "END"

    MMP_RULES += AIWResource 


    browsercorelibs.sources = BrowserCore.dll

    browsercorelibs.path = /sys/bin

    #browsercoreresources.sources = /epoc32/data/z/resource/apps/WrtTelService.rsc
    #browsercoreresources.path = /resource/apps

    DEPLOYMENT += browsercorelibs 
    #browsercoreresources
}

UTILITIES_DIR = $$ROOT_DIR/utilities


CONFIG += \
    building-libs \
    depend_includepath \
    dll
    
contains(br_mobility_sysinfo, yes) {
    DEFINES += QT_MOBILITY_SYSINFO
}

contains(br_mobility_bearer, yes) {
    lessThan(QT_VERSION, 4.7.0) {
        CONFIG += mobility
        MOBILITY = bearer
    }
    DEFINES += QT_MOBILITY_BEARER_MANAGEMENT
}

contains(br_mobility_serviceframework, yes) {
    CONFIG += mobility
    MOBILITY = serviceframework
}

# QtHighway is used in TB10.1 for Application Interworking (AIW) support.
contains(br_qthighway, yes) {
    DEFINES += QTHIGHWAY
    LIBS += -lxqservice -lxqserviceutil
}

contains(QT_CONFIG, embedded): CONFIG += embedded

!CONFIG(QTDIR_build) {
     OBJECTS_DIR = $$WRT_OUTPUT_DIR/browsercore/tmp
     DESTDIR = $$WRT_OUTPUT_DIR/bin
     MOC_DIR=$$WRT_OUTPUT_DIR/browsercore/tmp
     RCC_DIR=$$WRT_OUTPUT_DIR/browsercore/tmp
     TEMPDIR=$$WRT_OUTPUT_DIR/browsercore/build
     QMAKE_LIBDIR=$$DESTDIR $$QMAKE_LIBDIR
}

CONFIG(release, debug|release):!CONFIG(QTDIR_build){
    contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols
    unix : contains(QT_CONFIG, reduce_relocations): CONFIG += bsymbolic_functions
}

contains(br_tiled_backing_store, yes) {
    DEFINES += BEDROCK_TILED_BACKING_STORE
}


CONFIG -= warn_on
*-g++* : QMAKE_CXXFLAGS += -Wreturn-type -fno-strict-aliasing

CONFIG(gcov)   {
   LIBS +=   -lgcov
   QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
}

# Pick up 3rdparty libraries from INCLUDE/LIB just like with MSVC
win32-g++ {
    TMPPATH = $$quote($$(INCLUDE))
    QMAKE_INCDIR_POST += $$split(TMPPATH,";")
    TMPPATH = $$quote($$(LIB))
    QMAKE_LIBDIR_POST += $$split(TMPPATH,";")
}

symbian: {
    DEFINES += NO_IOSTREAM
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000 // Min 128kB, Max 32MB
    DEFINES += _WCHAR_T_DECLARED
    QMAKE_CXXFLAGS.CW = -O1 -wchar_t on
    TARGET.CAPABILITY = All -TCB -DRM -AllFiles 
    TARGET.UID3 = 0x200267BB
    TARGET.VID = VID_DEFAULT
    MMP_RULES += EXPORTUNFROZEN
}


# Import pre-built binary components.
symbian: {
    contains(browser_addon, no) {
      DEFINES += USE_DOWNLOAD_MANAGER=1
      INCLUDEPATH += /epoc32/include/applications
      INCLUDEPATH += /epoc32/include/platform/mw/cwrt
      LIBS += -lWrtDownloadMgrIpc
      LIBS += -lWrtDownloadMgr
   }
} else {
    include($$PWD/../../../../../import/import.pri)
}


#
# SOURCE and HEADER lists
#

include(core.pri)
include($$PWD/../appfw/appfw-includepath.pri)

# TEMP until appfw is its own dll
# DEFINES += BUILDING_BWF_CORE
include(../appfw/appfw.pri)

QT += network
QT += webkit

#
# INCLUDE PATHS
#
INCLUDEPATH += \
    $$PWD \
    $$PWD/network \
    $$PWD/../../bedrockProvisioning \
    #FIXME_10.1 fix path below
    $$PWD/../../../../mw/browser/bookmarksengine/browsercontentdll/inc
symbian: {
    INCLUDEPATH +=  $$PWD $$MW_LAYER_SYSTEMINCLUDE $$APP_LAYER_SYSTEMINCLUDE
#   INCLUDEPATH += /epoc32/include/oem/tgfw
}


symbian:MMP_RULES += SMPSAFE
