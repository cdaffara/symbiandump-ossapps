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

HEADERS += \
    $$PWD/network/networkerrorreply.h \
    $$PWD/network/webcookiejar.h \
    $$PWD/network/webnetworkaccessmanager.h \
    $$PWD/network/SchemeHandlerBr.h \
    $$PWD/network/SchemeHandlerBr_p.h \
    $$PWD/actionjsobject.h \
    $$PWD/browserpagefactory.h \
    $$PWD/brtglobal.h \
    $$PWD/scriptobjects.h \
    $$PWD/webpagedata.h \
    $$PWD/wrtbrowsercontainer_p.h \
    $$PWD/wrtbrowsercontainer.h

SOURCES += \
    $$PWD/network/networkerrorreply.cpp \
    $$PWD/network/webcookiejar.cpp \
    $$PWD/network/webnetworkaccessmanager.cpp \
    $$PWD/network/SchemeHandlerBr.cpp \
    $$PWD/actionjsobject.cpp \
    $$PWD/wrtbrowsercontainer.cpp
    
contains(br_mobility_bearer, yes) {
HEADERS += \
    $$PWD/network/WebNetworkConnectionManager.h \
    $$PWD/network/WebNetworkSession.h \
    $$PWD/singleton.h
SOURCES += \
    $$PWD/network/WebNetworkConnectionManager.cpp \
    $$PWD/network/WebNetworkSession.cpp \
}

# QtHighway is used in TB10.1 for Application Interworking (AIW) support.
contains(br_qthighway, yes) {
HEADERS += \
    $$PWD/network/SchemeHandlerBrQtHighway_p.h
SOURCES += \
    $$PWD/network/SchemeHandlerBrQtHighway_p.cpp
}

# Use default scheme handler if not using QtHighway.
contains(br_qthighway, no) {
HEADERS += \
    $$PWD/network/SchemeHandlerBrDefault_p.h
SOURCES += \
    $$PWD/network/SchemeHandlerBrDefault_p.cpp

symbian: {
SOURCES += $$PWD/network/WrtTelServiceBr.cpp 
HEADERS += \
   $$PWD/network/WrtTelServiceBr.h \
   $$PWD/network/WrtTelServiceBr_p.h 
}
}
