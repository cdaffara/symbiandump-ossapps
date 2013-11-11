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
TARGET = unittest_calendayview

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
				
INCLUDEPATH += . \
               ../../../inc/ \
               ../../../../inc/ \
               ../../../../traces/ \
               ../../../../../inc/ \
               

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/ \
               ../../../../inc/ \
               ../../../../src/ \
               ../../../../../inc/ 

# Input
HEADERS +=	hbeffect.h \
			calendateutils.h \
			calenservices.h \
			calencontext.h \
			calennotificationhandler.h \
			calenview.h \
			calennativeview.h \
			hb_calencommands.hrh \
			calendocloader.h \
			calendarcrkeys.h \
			calenuid.h \
			calenpluginlabel.h \
			calendaymodelmanager.h \
			calendaycontentwidget.h \
			calendaycontentscrollarea.h \
			calendayhourscrollarea.h \
			calendayutils.h \
			calendayview.h \
			calencommon.h \
			calendayitemview.h \
			calendaycontainer.h \
			calendayeventspane.h \
			calendayitem.h \
			calendaystatusstrip.h 

SOURCES +=	unittest_calendayview.cpp \
			calendayview.cpp \
			calennativeview.cpp \
			calendaycontentwidget.cpp \
            calendayitemview.cpp \
            calendayutils.cpp \
            calendaycontainer.cpp \
            calendayeventspane.cpp \
            calendayitem.cpp \
            calendaystatusstrip.cpp

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
        LIBS += -lagendainterface \
    		-lxqsettingsmanager \
    		-lcalencommonutils \
                -lafservice            
}

# End of file	--Don't remove this.
