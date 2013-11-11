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
#  Description: calendayhourelement test project file
#

TEMPLATE = app
TARGET = unittest_calendayhourelement

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb

#uncoment it if want to get images from test where it is posible.
#it need folder in c:/unittest path
#DEFINES += SAVE_IMAGES  

DEFINES += CALENVIEWS_DLL
				
INCLUDEPATH += . \
               ../../../inc/ \
               ./../../../../inc/ \
               ./../../../../../inc/

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/  

# Input
HEADERS +=	calendayhourelementtest.h \
            calendayhourelement.h \
            calendayhourscrollarea.h \
            calendayutils.h
			

SOURCES +=	unittest_calendayhourelement.cpp \
			calendayhourelement.cpp \
			calendayhourscrollarea.cpp \
            calendayutils.cpp

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
            
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendayhourelement.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendayhourelement.iby)"
        
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        
    LIBS += -lagendainterface \
            -lcalencommonutils
}

# End of file	--Don't remove this.
