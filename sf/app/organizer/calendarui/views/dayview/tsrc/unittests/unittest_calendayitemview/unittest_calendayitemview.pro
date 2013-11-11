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
TARGET = unittest_calendayitemview

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
				
INCLUDEPATH += . \
               ../../../inc/ \
               ../../../src/  \
               ./../../../../inc/ \
               ./../../../../../inc/

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/  \
               ./../../../../inc/ \
               ./../../../../../inc/

# Input
HEADERS +=	calencontext.h \
            calendayinfo.h \
            calenservices.h \
            CalenUid.h \
            calendayitemview.h \
            caleninstanceid.h \
            calendaycontainer.h \
            calendayutils.h \
            hbinstance.h \
            calendaymodel.h \
            calendayeventspane.h \
            calendayitem.h 	\
            calendayview.h \
            calendaystatusstrip.h	

SOURCES +=	unittest_calendayitemview.cpp \
			calendayitemview.cpp \
            calendayutils.cpp \
            calendaycontainer.cpp  \
            calendayeventspane.cpp \
            calendayitem.cpp \
            calendaystatusstrip.cpp
            
symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
        LIBS += -lagendainterface \
            -lcalencommonutils
            
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendayitemview.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendayitemview.iby)"
}

# End of file	--Don't remove this.
