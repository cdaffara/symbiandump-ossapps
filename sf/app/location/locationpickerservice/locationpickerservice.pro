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
# Description: Location picker service project file

SERVICEAPP = app
TARGET = locationpickerservice
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
MOC_DIR = moc
CONFIG += hb


symbian: { 
    # Build.inf rules
    BLD_INF_RULES.prj_exports += \
         "$${LITERAL_HASH}include <platform_paths.hrh>" \
         "rom/locationpickerservice.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(locationpickerservice.iby)" \
         "rom/locationpickerservice_resources.iby  LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(locationpickerservice_resources.iby)"

    TARGET.UID3 = 0x2002C3AA 
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x14000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        16943040
    TARGET.CAPABILITY = ALL \
        -TCB

    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
}

# ##### qthighway
CONFIG += service
LIBS += -lxqservice \
    -lxqserviceutil \
    -lganeswidgets.dll \
    -lthumbnailmanagerqt.dll \
    -llocationdatalookupdb
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

# #### /qthighway

RESOURCES += ./resources/locationpicker.qrc 

TRANSLATIONS += lilpicker.ts

SOURCES += src/main.cpp \
    src/locationpickercontent.cpp \
    src/locationpickerappwindow.cpp \
    src/locationpickersearchview.cpp \
    src/locationpickerproxymodel.cpp \
    src/locationpickercollectionlistcontent.cpp \
    src/locationpickercollectioncontent.cpp \
    src/locationpickerservice.cpp \
    src/locationpickerdatamanager.cpp \
    src/locationpickerdatamanager_p.cpp \
    src/locationpickerview.cpp \
    src/locationpickerdocumentloader.cpp \
    src/hgwidgetdatamodel.cpp
    
    
    

HEADERS += inc/locationpickercontent.h \
    inc/locationpickerappwindow.h \
    inc/locationpickersearchview.h \
    inc/locationpickerproxymodel.h \
    inc/locationpickercollectionlistcontent.h \
    inc/locationpickercollectioncontent.h \
    inc/locationpickertypes.h \
    inc/locationpickerservice.h \
    inc/locationpickerdatamanager.h \
    inc/locationpickerdatamanager_p.h \
    inc/locationpickerview.h \
    inc/locationpickerdocumentloader.h \
    inc/hgwidgetdatamodel.h



symbian:MMP_RULES += SMPSAFE
