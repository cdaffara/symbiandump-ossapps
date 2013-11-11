#
# ============================================================================
#  Name        : contactsdatastoreextension.pro
#  Part of     : contactsdatastoreextension    
#  Description : Project file
#
#  Copyright © 2009 Nokia.  All rights reserved.
#  This material, including documentation and any related computer
#  programs, is protected by copyright controlled by Nokia.  All
#  rights are reserved.  Copying, including reproducing, storing,
#  adapting or translating, any or all of this material requires the
#  prior written consent of Nokia.  This material also contains
#  confidential information which may not be disclosed to others
#  without the prior written consent of Nokia.
# ============================================================================
# 


TEMPLATE = lib
TARGET = nsmlcontactsdatastoreextension
DEPENDPATH += ../../contactsdatastoreextension ../inc ../src
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE



DEFINES += BUILD_CONTACTSDATASTOREEXTENSION

PRIVATE_HEADERS += ./inc/nsmlcontactsdatastoreextension_p.h

# Implementation
SOURCES += ./src/nsmlcontactsdatastoreextension.cpp \
		   ./src/nsmlcontactsdatastoreextension_p.cpp
		   
HEADERS += \
       $$PRIVATE_HEADERS

qtAddLibrary(QtContacts)
qtAddLibrary(QtVersit)
qtAddLibrary(XQUtils)
LIBS += -lestor.dll \
		-lbafl.dll  \
		-lnsmldebug.dll \
		
		
defBlock = \
 "$${LITERAL_HASH}if defined(EABI)" \
 "DEFFILE ./def/eabicontactsdatastoreextension" \
 "$${LITERAL_HASH}elif defined(WINSCW)" \
 "DEFFILE ./def/bwinscwcontactsdatastoreextension" \
 "$${LITERAL_HASH}endif"
 
MMP_RULES += defBlock

symbian { 
	TARGET.UID3 = 0x2002DC83
	TARGET.EPOCSTACKSIZE = 0x14000
	TARGET.EPOCHEAPSIZE = "0x020000 0x800000"
	TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL    
}

symbian:MMP_RULES += SMPSAFE
