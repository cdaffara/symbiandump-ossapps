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

include(../../../../common.pri)
include(../../docml.pri)

LIBS += -lhsutils
LIBS += -lhsmenuserviceprovider
LIBS += -lcaclient
LIBS += -lcasoftwareregistry


CONFIG += hb console plugin mobility
MOBILITY = serviceframework

QT += testlib \
      xml \
      sql
HB += hbfeedback
DEFINES += MENUSTATES_UNITTEST HSDOMAINMODEL_TEST HSWIDGETMODEL_LIB

coverage:DEFINES += COVERAGE_MEASUREMENT

DEPENDPATH += .\
              ./src \
              ./inc \
              ../../src \
              ../../inc

INCLUDEPATH += .\
               ./inc \
               ../../inc \
               ../../../../../../homescreen/homescreenapp/inc \
			   ../../../../../../homescreen/homescreenapp/hsdomainmodel/inc \
               ../../../../../../homescreen/homescreenapp/serviceproviders/hsmenuserviceprovider/inc \ 
               ../../../../../../homescreen/homescreenapp/hsutils/inc

                   
win32: {
DEFINES += HSDOMAINMODEL_TEST
}

symbian {
    for(docmlFile, docmlFiles): DOCML+=../../$$docmlFile
    
    ### get rid of localization warnings
    CONFIG -= hb
    load(hb.prf)
    CONFIG -= symbian_i18n
    ###
    TARGET.UID3 = 0x20022F6B
    LIBS += -lPlatformEnv
    TARGET.CAPABILITY = All -TCB
    TARGET.EPOCSTACKSIZE = 0x14000 // 131kB
    TARGET.EPOCHEAPSIZE = 0x20000 0x1000000 // 128kB - 48MB
    
    LIBS += -lsif
    LIBS += -lscrclient

    BLD_INF_RULES.prj_testexports += "./data/MIDPTestIcons.jad c:/testing/data/t_hsmenuworkerstateplugin/installs/" \
     "./data/MIDPTestIcons.jar c:/testing/data/t_hsmenuworkerstateplugin/installs/"

    BLD_INF_RULES.prj_exports += "./data/MIDPTestIcons.jad c:/testing/data/t_hsmenuworkerstateplugin/installs/" \
     "./data/MIDPTestIcons.jar c:/testing/data/t_hsmenuworkerstateplugin/installs/"
     
    BLD_INF_RULES.prj_testexports += "./data/DS_Snow.jad c:/testing/data/t_hsmenuworkerstateplugin/installs/" \
     "./data/DS_Snow.jar c:/testing/data/t_hsmenuworkerstateplugin/installs/"

    BLD_INF_RULES.prj_exports += "./data/DS_Snow.jad c:/testing/data/t_hsmenuworkerstateplugin/installs/" \
     "./data/DS_Snow.jar c:/testing/data/t_hsmenuworkerstateplugin/installs/"
     
    exportArmSis.sources += ./data/MIDPTestIcons.jad \
        ./data/MIDPTestIcons.jar
    exportArmSis.sources += ./data/DS_Snow.jad \
        ./data/DS_Snow.jar
	exportArmSis.path = c:/testing/data/t_hsmenuworkerstateplugin/installs
	DEPLOYMENT += exportArmSis	

    CONFIG += symbian_test	
    coverage: CONFIG -= symbian_test
    }

    
RESOURCES += ../../$$qrcFile      


include(t_hsmenuworkerstateplugin.pri)





symbian:MMP_RULES += SMPSAFE
