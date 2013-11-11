/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/

#ifndef SEARCHBASEPATH_H
#define SEARCHBASEPATH_H

#ifdef Q_OS_SYMBIAN
        #define SEARCH_PLUGIN_PATH QString("/resource/qt/plugins/onlinesearch")
        #define SEARCH_PLUGIN_CONFIG_PATH QString("/resource/qt/plugins/onlinesearch/config")
#else 
	#ifdef _DEBUG
		#define SEARCH_PLUGIN_PATH QString("/onlinesearch/debug/bin")
		#define SEARCH_PLUGIN_CONFIG_PATH QString("/onlinesearch/debug/bin/config")
	#else
		#define SEARCH_PLUGIN_PATH QString("/onlinesearch/release/bin")
		#define SEARCH_PLUGIN_CONFIG_PATH QString("/onlinesearch/release/bin/config")
	#endif
#endif 

#endif /* SEARCHBASEPATH_H */
