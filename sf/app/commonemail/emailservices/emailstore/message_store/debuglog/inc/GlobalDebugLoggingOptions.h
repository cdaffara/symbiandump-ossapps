/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Logger global definitions.
*
*/


#ifndef _GLOBAL_DEBUG_LOGGING_OPTIONS_H_
#define _GLOBAL_DEBUG_LOGGING_OPTIONS_H_

// =============================================================================
// If __DEBUG_LOGGING_OPTIONS is defined in this file, it will override any
// project level logging options that are currently defined.  By default, the
// global logging options are commented so that projects can specify their own
// logging levels.  If a project does not explicitly set their own debug logging
// options, __DEBUG_LOG_SHOW_LINE_NUMBERS, __DEBUG_LOG_ERRORS, and __DEBUG_LOG_INFO
// are displayed.
//
// Available logging options include:
//
// __DEBUG_LOG_NONE
// __DEBUG_LOG_CONSTRUCT
// __DEBUG_LOG_DESTRUCT
// __DEBUG_LOG_ENTER
// __DEBUG_LOG_EXIT
// __DEBUG_LOG_ERRORS
// __DEBUG_LOG_INFO
// __DEBUG_LOG_DEBUG1
// __DEBUG_LOG_DEBUG2
// __DEBUG_LOG_DEBUG3
// __DEBUG_LOG_ALL
//
// =============================================================================
#ifdef _DEBUG

	// The following line should be un-commented to prevent compilation warning.  We are
	// redefining each project's __DEBUG_LOGGING_OPTIONS definition.
	//#undef __DEBUG_LOGGING_OPTIONS

	// The following line should be uncommented to enable showing trace debug line
	// numbers, error traces, and info traces.
	//#define __DEBUG_LOGGING_OPTIONS (__DEBUG_LOG_SHOW_LINE_NUMBERS|__DEBUG_LOG_ERRORS|__DEBUG_LOG_INFO)

#endif // _DEBUG

#endif // _GLOBAL_DEBUG_LOGGING_OPTIONS_H_

// END FILE GlobalDebugLoggingOptions.h

