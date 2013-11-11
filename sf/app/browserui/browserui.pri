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
#   BrowserUi - common settings - browserui.pri
#
#   any edits made here that are common to app and mw should also be made 
#   in the mw\browser\browser.pri file
#


isEmpty(_BROWSERUI_PRI_INCLUDED_): {

    _BROWSERUI_PRI_INCLUDED_=1

    symbian {
        CONFIG += debug_and_release
        MMP_RULES += EXPORTUNFROZEN
    }

    CONFIG(debug, debug|release) {
        SUBDIRPART = Debug
    } else {
        SUBDIRPART = Release
    }

    # Figure out the root of where stuff should go (this could be done via configure)

    symbian {
        OUTPUT_DIR = $$PWD
    } else {
        OUTPUT_DIR = $$PWD/../../..
    }    
    OBJECTS_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    MOC_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    RCC_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    UI_DIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/tmp
    TEMPDIR= $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/$$TARGET/build
    DESTDIR = $$OUTPUT_DIR/WrtBuild/$$SUBDIRPART/bin
    QMAKE_RPATHDIR = $$DESTDIR $$QMAKE_RPATHDIR
    QMAKE_LIBDIR = $$DESTDIR $$QMAKE_LIBDIR

    # Add the output dirs to the link path too
    LIBS += -L$$DESTDIR

    DEPENDPATH += .
    INCLUDEPATH += .

    QMAKE_CXXFLAGS_DEBUG += -g

    CONFIG(gcov)   {
       LIBS +=   -lgcov
       QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    }
    #########################
    # Feature Flags     
    #########################
    br_mobility_bearer=no
    br_mobility_sysinfo=no
    br_mobility_serviceframework=no
    br_fast_allocator=no
    br_default_iap=no
    br_orbit_ui=no
    br_openurl=no
    br_qthighway=no
    br_tiled_backing_store=no
    br_layout=default
    
    # For now Symbian only, Windows, Maemo will use the defaults    
    symbian {
       contains (browser_addon, fivezero) {
          br_fast_allocator=yes
          br_mobility_sysinfo=yes
          br_mobility_bearer=yes
       }
       contains (browser_addon, ninetwo) {
          br_fast_allocator=yes
          br_mobility_sysinfo=yes
          br_mobility_bearer=yes
       }
       isEmpty (browser_addon){
          browser_addon=no
          br_mobility_sysinfo=yes
          br_mobility_bearer=yes
          br_mobility_serviceframework=yes
          br_orbit_ui=yes
          br_openurl=yes
          br_qthighway=yes
          br_layout=tenone
       }
    }
}

