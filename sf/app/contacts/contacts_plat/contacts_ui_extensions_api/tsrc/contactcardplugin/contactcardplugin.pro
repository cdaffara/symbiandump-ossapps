#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#
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
CONFIG += plugin hb
TARGET = $$qtLibraryTarget(contactcardplugin)
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += \
    contactcardextension.h \
    extensionfactory.h

SOURCES += \
    contactcardextension.cpp \
    extensionfactory.cpp

LIBS+= -lqtcontacts \
       -lhbcore \
symbian:
 {
    load(data_caging_paths)
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
#    TARGET.UID3 = 0x20027012
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    target.path = /sys/bin
    INSTALLS += target
    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $$QT_PLUGINS_BASE_DIR/contacts/extensions
    DEPLOYMENT += symbianplugin
    
}

target.path += $$[QT_INSTALL_PLUGINS]/contacts/extensions
INSTALLS += target