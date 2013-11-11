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

TEMPLATE = app

CONFIG += debug_and_release

QT += testlib

symbian: PLUGIN_SUBDIR = c:/private/2002C355

CONFIG(debug, debug|release) {
    DESTDIR = ./debug
    LIBS += -L../../../bin/debug -lhswidgetmodel
} else {
    DESTDIR = ./release
    LIBS += -L../../../bin/release -lhswidgetmodel
}

DEFINES += CONTACTWIDGET_TEST

symbian: LIBS += -lxqservice  -lthumbnailmanagerqt

CONFIG += hb mobility
MOBILITY = serviceframework contacts

INCLUDEPATH += ./inc \
               ../inc            


RESOURCES += ./t_contactwidgeths.qrc

DEPENDPATH += ./inc \
              ./src \
              ../inc \
              ../src \
              ../../inc

symbian {
    TARGET.UID3 = 0x2002C355
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCHEAPSIZE = 10000 54000000

    BLD_INF_RULES.prj_exports += \
        "./resource/test1_avatar.jpg     $$PLUGIN_SUBDIR/test1_avatar.jpg"   \
        "./resource/background_ok.png    $$PLUGIN_SUBDIR/background_ok.png" \
        "./resource/test_avatar_1600.bmp $$PLUGIN_SUBDIR/test_avatar_1600.bmp" \
        "./resource/test_avatar_1600.gif $$PLUGIN_SUBDIR/test_avatar_1600.gif" \
        "./resource/av_1600_comprlzw.tif $$PLUGIN_SUBDIR/av_1600_comprlzw.tif" \
        "./resource/av_1600_comprn.tif   $$PLUGIN_SUBDIR/av_1600_comprn.tif" \
        "./resource/test_avkeys_800x600.jpg $$PLUGIN_SUBDIR/test_avkeys_800x600.jpg" \
        "./resource/test_avkeys_600x800.jpg $$PLUGIN_SUBDIR/test_avkeys_600x800.jpg" \
        "./resource/test_avatar_640x480.jpg $$PLUGIN_SUBDIR/test_avatar_640x480.jpg" \
        "./resource/test_avatar_32x24.jpg $$PLUGIN_SUBDIR/test_avatar_32x24.jpg" \
        "./resource/test_avatar_type_text.txt $$PLUGIN_SUBDIR/test_avatar_type_text.txt"

}

include(t_contactwidgeths.pri)
