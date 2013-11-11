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
TARGET = unittest_calendaycontainer

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
HEADERS +=	calendaycontainertest.h \
            calendaycontainer.h \
            hbinstance.h \
            hbmainwindow.h \
			calendayutils.h \
			calendayeventspane.h \
			calendayitem.h \
			calendaystatusstrip.h \
			calendaymodel.h 
			

SOURCES +=	unittest_calendaycontainer.cpp \
			calendaycontainer.cpp  \
			calendayutils.cpp \
			calendayeventspane.cpp \
			calendayitem.cpp \
			calendaystatusstrip.cpp \
			calendaymodel.cpp

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
    		
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendaycontainer.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendaycontainer.iby)"
        
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        
    LIBS += -lagendainterface \
            -lcalencommonutils \
            -lxqsettingsmanager
}

# End of file	--Don't remove this.
