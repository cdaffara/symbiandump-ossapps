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
# Description: Project defnition file for controller.
#

TEMPLATE = lib
TARGET = calencontroller
DEPENDPATH += ./ \
		  				./inc \
		  				./src
DEFINES += CALENCONTROLLER_DLL
INCLUDEPATH += ./inc \
						   ../inc \
						   ../application/inc \
						   ../views/inc \
						   ../views/dayview/inc \
						   ../globaldata/inc \
						   ./traces

CONFIG += hb

symbian : {
	BLD_INF_RULES.prj_exports += \
		"./inc/calenactionuiutils.h  |../inc/calenactionuiutils.h" \
		"./inc/calenservicesimpl.h  |../inc/calenservicesimpl.h" \
		"./inc/calenservicesfactory.h |../inc/calenservicesfactory.h" \
		"./inc/calennotificationhandler.h |../inc/calennotificationhandler.h" \
		"./inc/calendocloader.h  |../inc/calendocloader.h" 							
	
	LIBS += -lcalinterimapi	\
					-lcalenviews	\
 					-lcalenglobaldata  \
 					-lcalencommonutils  \
 					-lcalencustomisationmanager \
					-lcaleneditor \
					-lbafl \
					-lagendainterface \
					-lagendaeventviewer \
					-lxqserviceutil \
					-lafservice 
						
 	TARGET.UID3 = 0x10282F25					  
  TARGET.CAPABILITY = CAP_GENERAL_DLL
  TARGET.EPOCALLOWDLLDATA = 1
  
  INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
}
# Input
HEADERS += calenactionui.h \
           calenactionuiutils.h \
           calenasynccallback.h \
           calencontroller.h \
           calendeleteui.h \
           calendeletingstate.h \
           calenidlestate.h \
           calennotificationhandler.h \
           calennotifier.h \
           calenpopulationstate.h \         
           calenservicesfactory.h \
           calenservicesimpl.h \
           calenstate.h \
           calenstatemachine.h \
           calenstatetable.h \
           calenviewmanager.h \
           calenhashfunctions.inl \
           calensettingsstate.h	\
           calenbackgroundstate.h	\
           calenexitingstate.h	\
           calenhelpstate.h	\
           calenprintingstate.h	\
           calensendingstate.h	\
           caleneditingstate.h	\
           calenviewingstate.h \
           caleneditui.h \
           calendocloader.h
           
SOURCES += calenactionui.cpp \
           calenactionuiutils.cpp \
           calenasynccallback.cpp \
           calencontroller.cpp \
           calendeleteui.cpp \
           calendeletingstate.cpp \
           calenidlestate.cpp \
           calennotifier.cpp \
           calenpopulationstate.cpp \
           calenservicesimpl.cpp \
           calenstate.cpp \
           calenstatemachine.cpp \
           calenviewmanager.cpp \
           calensettingsstate.cpp	\
           calenbackgroundstate.cpp	\
           calenexitingstate.cpp	\
           calenhelpstate.cpp	\
           calenprintingstate.cpp	\
           calensendingstate.cpp	\
           caleneditingstate.cpp	\
           calenviewingstate.cpp	\
           caleneditui.cpp \
           calendocloader.cpp
		   
RESOURCES += ./data/calendar.qrc

# End of file	--Don't remove this.   