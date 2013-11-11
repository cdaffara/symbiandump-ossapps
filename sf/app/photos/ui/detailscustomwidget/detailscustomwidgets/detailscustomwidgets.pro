#/* 
#* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
#* All rights reserved.
#* This component and the accompanying materials are made available
#* under the terms of "Eclipse Public License v1.0"
#* which accompanies this distribution, and is available
#* at the URL "http://www.eclipse.org/legal/epl-v10.html".
#*
#* Initial Contributors:
#* Nokia Corporation - initial contribution.
#*
#* Contributors:
#* 
#* Description:
#*
#*/ 

TEMPLATE = lib
TARGET = glxdetailscustomwidgets
CONFIG += hb
DEFINES += BUILD_DETAILSCUSTOM

win32{
CONFIG(release, debug|release){
  TARGET = glxdetailscustomwidgets
  DESTDIR = ../release # for easy plugin loading
}else{
  TARGET = glxdetailscustomwidgetsd
  DESTDIR = ../debug # for easy plugin loading
  }
}

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc

SOURCES += \
		   glxdetailsicon.cpp \
		   glxdetailstextedit.cpp

HEADERS += \
		   glxdetailsicon.h \
		   glxdetailstextedit.h
        
            
symbian {
    TARGET.UID3 = 0x2000A7BC
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    MMP_RULES += SMPSAFE 
    
    LIBS += -lbafl
}

defBlock = \      
"$${LITERAL_HASH}if defined(EABI)" \
"DEFFILE  ../eabi/glxdetailscustomwidgets.def" \
	 "$${LITERAL_HASH}else" \
	 "DEFFILE  ../bwins/glxdetailscustomwidgets.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

# End of file	--Don't remove this
