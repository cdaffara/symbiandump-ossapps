

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

DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT


CONFIG += ordered
SUBDIRS	    = group \
              loggers \
			  commonutilities \
			  tvout\tvoutwrapper \
			  ui \
              main \
			  gallery
              
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "rom/photos.iby   CORE_APP_LAYER_IBY_EXPORT_PATH(photos.iby)" \
        "rom/photos_resources.iby   LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(photos_resources.iby)" \
		"gallery/sis/photos_stub.sis /epoc32/data/z/system/install/photos_stub.sis" \
