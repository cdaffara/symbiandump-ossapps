#
#  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
#  All rights reserved.
#  This component and the accompanying materials are made available
#  under the terms of "Eclipse Public License v1.0"
#  which accompanies this distribution, and is available
#  at the URL "http://www.eclipse.org/legal/epl-v10.html".
#  
#  Initial Contributors:
#  Nokia Corporation - initial contribution.
#  
#  Contributors: 
#  
#  Description: calendaycontentscrollarea test project file
#

TEMPLATE = app
TARGET = unittest_calendaycontentscrollarea

DEFINES += CALENVIEWS_DLL

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
				
INCLUDEPATH += . \
               ../../../inc/ \
               ./../../../../inc/ \
               ./../../../../../inc/

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/  \
               ./../../../../inc/ \
               ./../../../../../inc/

# Input
HEADERS +=	calendaycontentscrollarea.h \
            calendayutils.h \
            hbinstance.h \
            hbmainwindow.h \
            hbswipegesture.h

SOURCES +=	unittest_calendaycontentscrollarea.cpp \
			calendaycontentscrollarea.cpp \
			calendayutils.cpp

RESOURCES += ../../../resources/calendaywidgets.qrc

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
            
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendaycontentscrollarea.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendaycontentscrollarea.iby)"
        
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        
    LIBS += -lagendainterface \
            -lcalencommonutils
}

# End of file	--Don't remove this.
