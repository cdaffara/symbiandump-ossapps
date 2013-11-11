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

TEMPLATE = app
CONFIG += hb mobility
MOBILITY = serviceframework

include(../../../../common.pri)
include(../../docml.pri)

LIBS += -lhsdomainmodel
LIBS += -lhsutils
LIBS += -lhsmenuserviceprovider
LIBS += -lcaclient

QT += testlib \
      xml \
      sql

DEFINES += MENUSTATES_UNITTEST

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH += .\
              ./src \
              ./inc \
              ./../src \
              ./../inc \
              ../../src \
              ../../inc
              
INCLUDEPATH += .\
               ./inc \
               ./../inc \
               ../../inc \
               ../../../hsmenuworkerstateplugin/inc \
               ../../../../serviceproviders/hsmenuserviceprovider/inc \               
               ../../../../hsutils/inc \
               ../../../../hsdomainmodel/inc/
               
symbian {
    for(docmlFile, docmlFiles): DOCML+=../../$$docmlFile
    ### get rid of localization warnings
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    ###
    TARGET.UID3 = 0x2002DCF4
    TARGET.CAPABILITY = CAP_APPLICATION AllFiles
    TARGET.EPOCSTACKSIZE = 0x14000 // 80kB
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 16MB
    CONFIG += symbian_test
    coverage: CONFIG -= symbian_test
    exists($${EPOCROOT}epoc32/include/platform/mw/XQSettingsManager) {
        LIBS += -lxqsettingsmanager
    }    
}

RESOURCES += ../../$$qrcFile

include(t_installedappsstate.pri)
                     

symbian:MMP_RULES += SMPSAFE
