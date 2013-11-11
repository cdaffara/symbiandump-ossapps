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

TEMPLATE = app
TARGET = videotelui
CONFIG += hb
TRANSLATIONS = video_telephone.ts

INCLUDEPATH += ../../inc
# Input
HEADERS += lcapplication.h
SOURCES += lcapplication.cpp main.cpp

symbian: {

		:BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
		:BLD_INF_RULES.prj_exports += "./rom/videotelui.iby CORE_APP_LAYER_IBY_EXPORT_PATH(videotelui.iby)"
		:BLD_INF_RULES.prj_exports += "./rom/videoteluiResources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(videoteluiResources.iby)"		
		:BLD_INF_RULES.prj_exports += "./conf/s60videotelephony.confml APP_LAYER_CONFML(s60videotelephony.confml)"
		:BLD_INF_RULES.prj_exports += "./conf/CI_s60videotelephony.confml APP_LAYER_CONFML(CI_s60videotelephony.confml)"
		:BLD_INF_RULES.prj_exports += "./conf/s60videotelephony_102072D0.crml APP_LAYER_CRML(s60videotelephony_102072D0.crml)"
		:BLD_INF_RULES.prj_exports += "./conf/s60videotelephony_10281872.crml APP_LAYER_CRML(s60videotelephony_10281872.crml)"
		:BLD_INF_RULES.prj_exports += "./inc/VideoTelephonyVariant.hrh  |../../inc/videotelephonyvariant.hrh"
		:BLD_INF_RULES.prj_exports += "./inc/VideoTelephonyInternalCRKeys.h  |../../inc/videotelephonyinternalcrkeys.h"
		:BLD_INF_RULES.prj_exports += "./data/videotelui_stub.sis /epoc32/data/z/system/install/videotelui_stub.sis"		
		
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x101F8681
    TARGET.VID = 0x101FB657
    TARGET.CAPABILITY = CAP_APPLICATION NetworkControl SwEvent Location MultimediaDD CommDD
    LIBS += -llivecommsui -lvtlogger -lapgrfx -lcone
    RSS_RULES += "hidden = KAppIsHidden;"
}
