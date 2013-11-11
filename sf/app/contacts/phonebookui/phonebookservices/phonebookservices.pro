# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = app
TARGET = phonebookservices
CONFIG += hb
CONFIG += service

DEPENDPATH += . \
    src

INCLUDEPATH +=  ../cntcommonui/core \
                ../cntcommonui/common \
                ../cntcommonui/views \
                ../cntcommonui/contacteditors \
                ../cntcommonui/contactcard \
                ../cntcommonui/collections \
                ../cntcommonui/widgets \
                ../../inc
                
INCLUDEPATH += ../../phonebookengines/cntlistmodel/inc \
               ../../phonebookengines/cntimageutility/inc \

INCLUDEPATH += . \
    inc \
    ../../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

# Input
HEADERS += inc/cntserviceviewmanager.h \
           inc/cntserviceproviderold.h \
           inc/cntserviceproviderold2.h \
           inc/cntservicecontactfetchview.h \
           inc/cntservicecontactselectionview.h \
           inc/cntserviceeditview.h \
           inc/cntservicecontactcardview.h \
           inc/cntservicegroupmemberview.h \
           inc/cntserviceassigncontactcardview.h \
           inc/cntserviceviewfactory.h \
           inc/cntservices.h \
           inc/cntabstractserviceprovider.h \
           inc/cntserviceproviderfetch.h \
           inc/cntserviceproviderviewing.h \
           inc/cntserviceprovideredit.h \
           inc/cntserviceviewparams.h \
           inc/cntservicesubeditview.h

SOURCES += src/main.cpp \
           src/cntserviceviewmanager.cpp \
           src/cntserviceproviderold.cpp \
           src/cntserviceproviderold2.cpp \
           src/cntservicecontactfetchview.cpp \
           src/cntservicecontactselectionview.cpp \
           src/cntserviceeditview.cpp \
           src/cntservicecontactcardview.cpp \
           src/cntservicegroupmemberview.cpp \
           src/cntserviceassigncontactcardview.cpp \
           src/cntserviceviewfactory.cpp \
           src/cntservices.cpp \
           src/cntserviceproviderfetch.cpp \
           src/cntserviceproviderviewing.cpp \
           src/cntserviceprovideredit.cpp \
           src/cntservicesubeditview.cpp

# capability
TARGET.CAPABILITY = CAP_APPLICATION NetworkControl

TRANSLATIONS = contacts.ts

TARGET.UID3 = 0x2002429B

LIBS += -lxqservice \
        -lxqserviceutil \
        -lhbcore \
        -lqtcontacts \
        -lcntlistmodel \
        -lcntcommonui \
        -lqtversit \
        -lcntimageutility

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden
symbian:MMP_RULES += SMPSAFE