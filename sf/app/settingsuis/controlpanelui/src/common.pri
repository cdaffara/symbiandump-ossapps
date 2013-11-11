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
# Description: controlpanel project - common qmake settings
#

CONFIG += debug_and_release 

# On win32 and mac, debug and release libraries are named differently.
# We must follow the debug and release settings Qt was compiled with:
# build debug iff Qt built debug, build release iff Qt built release.

win32|mac {
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug):  CONFIG+=debug
        contains(QT_CONFIG,release):CONFIG+=release
    }
}

CONFIG(debug, debug|release) {
    SUBDIRPART = debug
} else {
    SUBDIRPART = release
}

win32 { 
    DESTDIR = C:/ControlPanel/$$SUBDIRPART/bin
    OBJECTS_DIR = $$PWD/../$$SUBDIRPART/tmp/$$TARGET
}

# Add the output dirs to the link path too
LIBS += -L$$DESTDIR

MOC_DIR = moc
OBJECT_DIR = obj
RCC_DIR = rcc

#For some reason the default include path doesn't include MOC_DIR on symbian
symbian {
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MW_LAYER_PLATFORM_EXPORT_PATH(cplogger)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
}

INCLUDEPATH += $$PWD/inc
win32 {
    # add platfrom API for windows
    INCLUDEPATH += $$PWD/../../controlpanel/controlpanel_plat/inc
}



