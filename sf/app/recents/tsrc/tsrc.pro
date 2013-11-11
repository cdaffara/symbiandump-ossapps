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

SUBDIRS +=  \
            ./../logsui/logsengine/tsrc/ut_logsengine \
            ./../logsui/logsengine/logssymbianos/tsrc/ut_logssymbianos \
            ./../logsui/logscntfinder/tsrc/ut_logscntfinder \
            ./../logsui/logsapp/tsrc/ut_logsapp
		
