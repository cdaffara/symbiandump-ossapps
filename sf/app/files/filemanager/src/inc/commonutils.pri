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
# Description: 
#

INTERNAL_HEADERS += $$PWD/fmutils.h \
                    $$PWD/fmdrivemodel.h \
                    $$PWD/fmfileiconprovider.h \
                    $$PWD/fmdrivewatcher/fmdrivewatcher.h \
                    $$PWD/fmfiletyperecognizer.h \
                    $$PWD/fmfilesystemproxymodel.h \
                    $$PWD/fmserviceutils/fmserviceutils.h
                    
INTERNAL_SOURCES += $$PWD/fmdrivemodel.cpp \
                    $$PWD/fmfileiconprovider.cpp \
                    $$PWD/fmdrivewatcher/fmdrivewatcher.cpp \
                    $$PWD/fmfiletyperecognizer.cpp \
                    $$PWD/fmutils.cpp \
                    $$PWD/fmfilesystemproxymodel.cpp \
                    $$PWD/fmserviceutils/fmserviceutils.cpp

symbian {
    INTERNAL_HEADERS += $$PWD/fms60utils.h \
                        $$PWD/fmdrivewatcher/private/symbian/fmdrivewatcherprivate.h \
                        $$PWD/fmdrivewatcher/private/symbian/fmdriveevent.h \
                        $$PWD/fmserviceutils/private/symbian/fmserviceutilsprivate.h \
                        $$PWD/fmserviceutils/private/symbian/fmserviceutilshandler.h

    INTERNAL_SOURCES += $$PWD/fmutils_s60.cpp \
                        $$PWD/fms60utils.cpp \
                        $$PWD/fmdrivewatcher/private/symbian/fmdrivewatcherprivate.cpp \
                        $$PWD/fmdrivewatcher/private/symbian/fmdriveevent.cpp \
                        $$PWD/fmserviceutils/private/symbian/fmserviceutilsprivate.cpp \
                        $$PWD/fmserviceutils/private/symbian/fmserviceutilshandler.cpp
}
win32 {
    INCLUDEPATH += $$PWD/fmdrivewatcher/
    INCLUDEPATH += $$PWD/fmdrivewatcher/private/win32/
    INCLUDEPATH += $$PWD/fmserviceutils/
    INCLUDEPATH += $$PWD/fmserviceutils/private/win32/

    INTERNAL_HEADERS += $$PWD/fmdrivewatcher/private/win32/fmdrivewatcherprivate.h \
                        $$PWD/fmserviceutils/private/win32/fmserviceutilsprivate.h

    INTERNAL_SOURCES += $$PWD/fmutils_win.cpp \
                        $$PWD/fmdrivewatcher/private/win32/fmdrivewatcherprivate.cpp \
                        $$PWD/fmserviceutils/private/win32/fmserviceutilsprivate.cpp

}

HEADERS += $$INTERNAL_HEADERS
SOURCES += $$INTERNAL_SOURCES