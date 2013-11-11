# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Project definition include file for Day View

# Input

# Enables panning-effect when swiching between days
# Uncomment to enable
#DEFINES	+= CALENDAYVIEW_PANNING_ENABLED

INCLUDEPATH += ./dayview/inc \
              $${EPOCROOT}sf/mw/hb/include/hbwidgets/private

DEPENDPATH += ./dayview/inc \
			  ./dayview/src			 
 
HEADERS += calendayview.h \
		   calendaycommonheaders.h \
		   calendayutils.h \
		   calendaycontentscrollarea.h \
		   calendayhourscrollarea.h \
		   calendaycontentwidget.h \
		   calendaymodelmanager.h \
		   calendaymodel.h \
		   calendayitemview.h \
		   calendaycontainer.h \
		   calendayinfo.h \
		   calendayitem.h \
		   calendayhourelement.h \
		   calendaystatusstrip.h \
		   calendayeventspane.h
		   
SOURCES += calendayview.cpp \
		   calendayutils.cpp \
		   calendaycontentscrollarea.cpp \
		   calendayhourscrollarea.cpp \
		   calendaycontentwidget.cpp \
		   calendaymodelmanager.cpp \
		   calendaymodel.cpp \
		   calendayitemview.cpp \
		   calendaycontainer.cpp \
		   calendayinfo.cpp \
		   calendayitem.cpp \
		   calendayhourelement.cpp \
		   calendaystatusstrip.cpp \
		   calendayeventspane.cpp
		   
RESOURCES += dayview/resources/calendaywidgets.qrc

# End of file	--Don't remove this.