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
# Description:                                                        
#                                                                     
#

TEMPLATE = lib

TARGET = logsservices
CONFIG += dll

INCLUDEPATH += ./
INCLUDEPATH += ../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input

SOURCES += src/logsservices.cpp

DEFINES += LOGSSERVICES_LIB

libFiles.sources = logsservices.dll
libFiles.path = "!:/sys/bin"
DEPLOYMENT += libFiles

symbian: {
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x10282EBB

    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lxqservice  -lxqserviceutil

    defFiles = "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/logsservices.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE eabi/logsservices.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defFiles
    MMP_RULES += SMPSAFE
}
