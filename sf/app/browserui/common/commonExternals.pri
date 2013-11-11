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
# Setup common settings for all demos
#

# Everything should be relative to ROOT_DIR (makes cut-and-paste safer).
ROOT_DIR = $$PWD/../../../..


#
# set lib dirs and output dirs depending on debug or not
symbian {
    CONFIG(release, debug|release) {
        APP_OUTPUT_DIR=$$PWD/../WrtBuild/Release
    }
    CONFIG(debug, debug|release) {
        APP_OUTPUT_DIR=$$PWD/../WrtBuild/Debug
    }
} else {
    CONFIG(release, debug|release) {
        APP_OUTPUT_DIR=$$ROOT_DIR/WrtBuild/Release
    }
    CONFIG(debug, debug|release) {
        APP_OUTPUT_DIR=$$ROOT_DIR/WrtBuild/Debug
    }
}
OBJECTS_DIR = $$APP_OUTPUT_DIR/browser/tmp
DESTDIR = $$APP_OUTPUT_DIR/bin
DLLDIR = $$APP_OUTPUT_DIR/bin
MOC_DIR=$$APP_OUTPUT_DIR/browser/tmp
RCC_DIR=$$APP_OUTPUT_DIR/browser/tmp
TEMPDIR=$$APP_OUTPUT_DIR/browser/build

# use the stylesheets for demo UIs!
!contains(DEFINES, WRT_USE_STYLE_SHEET=.): DEFINES += WRT_USE_STYLE_SHEET=1

# no app bundle (for OSX)
CONFIG -= app_bundle

# create QMAKE_RPATHDIR, LIBDIR
QMAKE_RPATHDIR = \
    $$DLLDIR \
    $$QMAKE_RPATHDIR

QMAKE_LIBDIR = \
    $$DLLDIR \
    $$QMAKE_RPATHDIR

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
    include($$ROOT_DIR/import/import.pri)
}

# BedrockProvisioning
INCLUDEPATH += $$ROOT_DIR/sf/app/browserui/bedrockProvisioning
LIBS += -lBedrockProvisioning

# Bookmarks
#LIBS += -lBookMarksClient
LIBS += -lbookmarksapi
LIBS += -lbrowsercontentdll

# Browsercore
BROWSERCORE_DIR = $$ROOT_DIR/sf/app/browserui/browsercore
INCLUDEPATH += $$BROWSERCORE_DIR/core
INCLUDEPATH += $$BROWSERCORE_DIR/appfw/Common


include($$BROWSERCORE_DIR/appfw/Api/Api.pri)
LIBS += -lBrowserCore

# Qt
QT += xml network sql webkit
