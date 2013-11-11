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
* Description:  This file defines logging macros for Provisioning
*
*/



#ifndef PROVISIONINGDEBUG_H
#define PROVISIONINGDEBUG_H

#include <e32debug.h> 

//Examples
//FLOG( _L( "[Provisioning] ProvisioningEngine::ConstructL:" ) );
//FTRACE(RDebug::Print(_L("[Provisioning] ProvisioningEngine::ConstructL: err (%d)"), err));
#ifdef _DEBUG

const TInt KBTHexDumpWidth=16;
const TInt KBTLogBufferSize=100;
const TText KFullStopChar='.';

_LIT(KBTFirstFormatString,"%04x: ");
_LIT(KBTSecondFormatString,"%02x ");
_LIT(KBTThirdFormatString,"%c");
_LIT(KBTThreeSpaces,"   ");
_LIT(KBTSeparator," ");

inline void HexDump(const TUint8* aPtr, TInt aLen)
    {
	if( aPtr != NULL )
	    {
    	TBuf<KBTLogBufferSize> line;
	    TInt i = 0;

    	while (aLen>0)
    		{
    		TInt n = (aLen>KBTHexDumpWidth ? KBTHexDumpWidth : aLen);
    		line.AppendFormat(KBTFirstFormatString,i);
    		TInt j;
    		for (j=0; j<n; j++)
    			line.AppendFormat(KBTSecondFormatString,aPtr[i+j]);
    		while (j++<KBTHexDumpWidth)
    			line.Append(KBTThreeSpaces);
    		line.Append(KBTSeparator);
    		for (j=0; j<n; j++)
    			line.AppendFormat(KBTThirdFormatString,(aPtr[i+j]<32 || aPtr[i+j]>126 || aPtr[i+j]==37) ? KFullStopChar : aPtr[i+j]);

    		RDebug::Print(line);

            line.Zero();
    		aLen-=n;
    		i+=n;
    	    }
        }
    }


// ------------------------------------------

inline void FHex(const TUint8* aPtr, TInt aLen)
    {
    HexDump( aPtr, aLen );
    }

// ------------------------------------------

inline void FHex(const TDesC8& aDes)
    {
    HexDump(aDes.Ptr(), aDes.Length());
    }

// ===========================================================================
#ifdef __WINS__     // File logging for WINS
// ===========================================================================
#include <e32std.h>
#include <f32file.h>
#include <flogger.h>

_LIT( KLogFile, "Provisioning.log" );
_LIT( KLogDirFullName, "c:\\logs\\" );
_LIT( KLogDir, "Provisioning" );

#define FLOG( a )   { FPrint(a); }
#define FTRACE( a ) { a; }

// Declare the FPrint function
//
inline void FPrint( const TRefByValue<const TDesC> aFmt, ... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    RFileLogger::WriteFormat( KLogDir,
                              KLogFile,
                              EFileLoggingModeAppend,
                              aFmt,
                              list );
    }

// ===========================================================================
#else               // RDebug logging for target HW
// ===========================================================================
#include <e32svr.h>

#define FLOG( a )   { RDebug::Print( a ); }
#define FTRACE( a ) { a; }

#endif //__WINS__

// ===========================================================================
#else // // No loggings --> Reduced binary size
// ===========================================================================
#define FLOG( a )
#define FTRACE( a )

#endif // _DEBUG

#endif      // PROVISIONINGDEBUG_H
            
// End of File
