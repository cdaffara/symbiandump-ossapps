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
#  Description: calendaycontentwidget test project file
#

TEMPLATE = app
TARGET = unittest_calendayhourscrollarea

#uncoment it if want to get images from test where it is posible.
#it need folder in c:/unittest path
#DEFINES += SAVE_IMAGES 

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
HEADERS +=	calendayhourelement.h \
            calendayhourscrollarea.h \
            calendayutils.h
			

SOURCES +=	unittest_calendayhourscrollarea.cpp \
			calendayhourscrollarea.cpp \
			calendayutils.cpp

RESOURCES += ../../../resources/calendaywidgets.qrc

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
            
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendayhourscrollarea.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendayhourscrollarea.iby)"
        
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        
    LIBS += -lagendainterface \
            -lcalencommonutils
}

# End of file	--Don't remove this.
