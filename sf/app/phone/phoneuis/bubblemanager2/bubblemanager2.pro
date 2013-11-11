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

TEMPLATE = subdirs
CONFIG += ordered

symbian: {
    load(data_caging_paths)

    include(./inc/api_headers.pri)

    headers.sources = $$BUBBLEMGR2_API_HEADERS
    headers.path = |../../inc
    for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$headers.path/$$basename(header)"
    
    BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "./rom/bubblemanager.iby    CORE_APP_LAYER_IBY_EXPORT_PATH(bubblemanager.iby)"
}

SUBDIRS += ./bubblecore
