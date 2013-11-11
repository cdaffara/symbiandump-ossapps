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
TARGET = unittest_calendayeventspane

#uncoment it if want to get images from test where it is posible.
#it need folder in c:/unittest path
#DEFINES += SAVE_IMAGES 

MOC_DIR = moc

CONFIG += qtestlib
CONFIG += symbian_test
CONFIG += hb
				
INCLUDEPATH += . \
               ../../../inc/ \

DEPENDPATH  += . \
               ../../../inc/ \
               ../../../src/  

# Input
HEADERS +=	calendayeventspane.h 
			

SOURCES +=	unittest_calendayeventspane.cpp \
			calendayeventspane.cpp 

RESOURCES += ../../../resources/calendaywidgets.qrc

symbian : {
	TARGET.CAPABILITY = CAP_APPLICATION
    MMP_RULES += "USERINCLUDE ."
            
    BLD_INF_RULES.prj_testexports += \
        "./rom/unittest_calendayeventspane.iby        CORE_APP_LAYER_IBY_EXPORT_PATH(unittest_calendayeventspane.iby)"
}

# End of file	--Don't remove this.
