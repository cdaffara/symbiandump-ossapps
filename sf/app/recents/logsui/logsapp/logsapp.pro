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
#

TEMPLATE = app
TARGET = logs
CONFIG += hb
CONFIG += service
RESOURCES += logsapp.qrc
TRANSLATIONS = dialer.ts

INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ../logsengine
INCLUDEPATH += ../logsengine/inc
INCLUDEPATH += ../logsengine/logssymbianos/inc

INCLUDEPATH += ../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
HEADERS += inc/logsmainwindow.h
HEADERS += inc/logsapplication.h
HEADERS += inc/logsviewmanager.h
HEADERS += inc/logsbaseview.h
HEADERS += inc/logsrecentcallsview.h
HEADERS += inc/logsdetailsview.h
HEADERS += inc/logsmatchesview.h
HEADERS += inc/logsservicehandler.h
HEADERS += inc/logsservicehandlerold.h
HEADERS += inc/logseffecthandler.h
HEADERS += inc/logspageindicator.h
HEADERS += inc/logspageindicatoritem.h
HEADERS += inc/logscomponentrepository.h
HEADERS += inc/logsappsettings.h
HEADERS += ../logsengine/logssymbianos/inc/logsforegroundwatcher.h

SOURCES += src/main.cpp 
SOURCES += src/logsmainwindow.cpp
SOURCES += src/logsapplication.cpp
SOURCES += src/logsviewmanager.cpp
SOURCES += src/logscomponentrepository.cpp
SOURCES += src/logsbaseview.cpp
SOURCES += src/logsrecentcallsview.cpp
SOURCES += src/logsdetailsview.cpp
SOURCES += src/logsmatchesview.cpp
SOURCES += src/logsservicehandler.cpp
SOURCES += src/logsservicehandlerold.cpp
SOURCES += src/logseffecthandler.cpp
SOURCES += src/logspageindicator.cpp
SOURCES += src/logspageindicatoritem.cpp
SOURCES += src/logsappsettings.cpp
SOURCES += ../logsengine/logssymbianos/src/logsforegroundwatcher.cpp
 
symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x101F4CD5
    TARGET.CAPABILITY = CAP_APPLICATION NetworkControl
    LIBS += -lxqservice  -lxqserviceutil -llogsengine -ldialpad -lxqkeycapture -ltstaskmonitorclient \
            -lxqsettingsmanager -lapparc -lcone
    SKINICON = qtg_large_dialer
    MMP_RULES += SMPSAFE
}

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable

DOCML += resources/recentCallsView.docml
DOCML += resources/matchesView.docml
DOCML += resources/detailsView.docml
