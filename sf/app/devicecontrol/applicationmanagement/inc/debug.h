/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application Management Debug macro definitions
*
*/


#ifndef DEBUG_HEADER_
#define DEBUG_HEADER_

// INCLUDES

#include <f32file.h>
#include <flogger.h>

// CONSTANTS
_LIT( KTarmLogFolderName, "tarm" );
_LIT( KTarmLogFileName, "tarm.log" );
// MACROS
//#define _TARM_FILE_DEBUG
#ifdef _TARM_FILE_DEBUG
		#define RDEBUG(X)					RFileLogger::Write( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X) )
		#define RDEBUG_2(X,Y)			RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X),Y )	
		#define RDEBUG_3(X,Y,Z)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X),Y,Z )	
		#define RDEBUG_4(X,Y,Z,A)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X),Y,Z,A )	
		#define RDEBUG_5(X,Y,Z,A,B)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L(X),Y,Z,A,B )
		
		#define RDEBUG8_2(X,Y)			RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L8(X),Y )	
		#define RDEBUG8_3(X,Y,Z)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L8(X),Y,Z )	
		#define RDEBUG8_4(X,Y,Z,A)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L8(X),Y,Z,A )	
		#define RDEBUG8_5(X,Y,Z,A,B)	  RFileLogger::WriteFormat( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, _L8(X),Y,Z,A,B )
		#define RDEBUG_HEX8(X,Y)	RFileLogger::HexDump( KTarmLogFolderName, KTarmLogFileName, EFileLoggingModeAppend, NULL, NULL, X,Y )	
		
#else

	#ifdef _DEBUG
		#define RDEBUG(X)								RDebug::Print(_L(X))
		#define RDEBUG_2(X,Y)						RDebug::Print(_L(X),Y)
		#define RDEBUG_3(X,Y,Z)					RDebug::Print(_L(X),Y,Z)
		#define RDEBUG_4(X,Y,Z,A)					RDebug::Print(_L(X),Y,Z,A)
		#define RDEBUG_5(X,Y,Z,A,B)					RDebug::Print(_L(X),Y,Z,A,B)
		
		#define RDEBUG8_2(X,Y)						RDebug::Printf(X,Y)
		#define RDEBUG8_3(X,Y,Z)					RDebug::Printf(X,Y,Z)
		#define RDEBUG8_4(X,Y,Z,A)					RDebug::Printf(X,Y,Z,A)
		#define RDEBUG8_5(X,Y,Z,A,B)					RDebug::Printf(X,Y,Z,A,B)
		
		#define RDEBUG_HEX8(X,Y)
		
	#else
		#define RDEBUG(X)								
		#define RDEBUG_2(X,Y)						
		#define RDEBUG_3(X,Y,Z)	
		#define RDEBUG_4(X,Y,Z,A)	
		#define RDEBUG_5(X,Y,Z,A,B)	
		
		#define RDEBUG8_2(X,Y)
		#define RDEBUG8_3(X,Y,Z)	
		#define RDEBUG8_4(X,Y,Z,A)			
		#define RDEBUG8_5(X,Y,Z,A,B)		
		
		#define RDEBUG_HEX8(X,Y)		
		
	#endif // _DEBUG
	
#endif // _TARM_FILE_DEBUG




#endif // DEBUG_HEADER_



