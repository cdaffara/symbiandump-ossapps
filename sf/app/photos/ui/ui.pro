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
TEMPLATE    = subdirs
CONFIG += ordered

exportfile += "inc/glxmodelroles.h APP_LAYER_PLATFORM_EXPORT_PATH(glxmodelroles.h)"
BLD_INF_RULES.prj_exports += exportfile

SUBDIRS	    = detailscustomwidget \
               uiengine \
              commandhandlers \
              viewutilities \
              widgets \
              views \
              viewmanagement \
#	      viewplugins
