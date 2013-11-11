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
TARGET =

DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += CNTLISTMODEL_NO_EXPORT
    
MOC_DIR = moc

QT += testlib xml

CONFIG += hb

TARGET.CAPABILITY = ALL \
    -TCB

INCLUDEPATH += .
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
HEADERS += inc/testrunner.h \
           inc/ut_cntlistmodel.h \
           inc/ut_cntcache.h \
           inc/ut_cntpresenceinfoprovider.h \
           inc/ut_cntdefaultinfoprovider.h \
           inc/ut_cntdisplaytextformatter.h \
           ../../inc/cntlistmodelglobal.h \
           ../../inc/cntlistmodel.h \
           ../../inc/cntlistmodel_p.h \
           ../../inc/cntcache.h \
           ../../inc/cntcache_p.h \
           ../../inc/cntdefaultinfoprovider.h \
           ../../inc/cntpresenceinfoprovider.h \
           ../../inc/cntdisplaytextformatter.h \
           ../../../../inc/cntdebug.h

SOURCES += src/testrunner.cpp \
           src/main.cpp \
           src/ut_cntlistmodel.cpp \
           src/ut_cntcache.cpp \
           src/ut_cntpresenceinfoprovider.cpp \
           src/ut_cntdefaultinfoprovider.cpp \
           src/ut_cntdisplaytextformatter.cpp \
           ../../src/cntlistmodel.cpp \
           ../../src/cntcache.cpp \
           ../../src/cntcache_p.cpp \
           ../../src/cntdefaultinfoprovider.cpp \
           ../../src/cntpresenceinfoprovider.cpp \
           ../../src/cntdisplaytextformatter.cpp

BLD_INF_RULES.prj_exports += "image1.png /epoc32/winscw/c/data/images/"
BLD_INF_RULES.prj_exports += "image2.png /epoc32/winscw/c/data/images/"
    
LIBS += -lQtContacts \
        -lhbcore \
        -lthumbnailmanagerqt \
        -lpresencecacheqt \
        -lxqsettingsmanager

