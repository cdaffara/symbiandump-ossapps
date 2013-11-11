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

TEMPLATE = subdirs
CONFIG += ordered

symbian: {  
    //:BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"    
    :BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"packetvideoengine/group/bld.inf\""
    :BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"vtprotocols/group/bld.inf\""   
    :BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"vtprotocolplugins/group/bld.inf\""
    :BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"vtengines/group/bld.inf\""
    :BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"vtuis/lcvtplugin/group/bld.inf\""
    :BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"videotelephony_plat/group/bld.inf\""
}

SUBDIRS += vtuis/lcvtindicatorplugin
SUBDIRS += vtuis/lcvtindicatorcontroller
SUBDIRS += vtuis/lcvtapp
