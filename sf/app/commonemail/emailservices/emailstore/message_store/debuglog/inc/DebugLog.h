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
* Description:  Logger interface.
*
*/


#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <f32file.h>

struct CLogFileHandler;

// ==========================================================================
// CLASS: RDebugLog
// ==========================================================================
class RDebugLog : public CBase
{
public:

    IMPORT_C RDebugLog();
    
    IMPORT_C virtual ~RDebugLog();
    
	IMPORT_C void Open( const TDesC&  aLogDirectory,
                        const TDesC8& aClassName8,
                        TAny*         aObject );

	IMPORT_C void SetCloseBetweenWrites( TBool aValue );

	IMPORT_C void Close();
	
	IMPORT_C void CleanupClosePushL();
	
	IMPORT_C void Write( const TDesC8& aFuncName8, const TDesC16& aText ) const;
	IMPORT_C void WriteFormat( const TDesC8& aFuncName8, TRefByValue< const TDesC16 > aFmt,... ) const;
	IMPORT_C void WriteFormat( const TDesC8& aFuncName8, TRefByValue< const TDesC16 > aFmt, VA_LIST& aList ) const;
	IMPORT_C void Write( const TDesC8& aFuncName8, const TDesC8& aText ) const;
	IMPORT_C void WriteFormat( const TDesC8& aFuncName8, TRefByValue< const TDesC8 > aFmt,... ) const;
	IMPORT_C void WriteFormat( const TDesC8& aFuncName8, TRefByValue< const TDesC8 > aFmt, VA_LIST& aList ) const;
	IMPORT_C void HexDump( const TDesC8& aFuncName8, const TDesC8& aBuffer ) const;

    IMPORT_C static void LogLeakedObjects();
    
    TPtrC8 ClassName8();
    
    TUint32 ObjectAddress();

private: 

    TBool PrepareForWrite();
    void WriteComplete();
    
    static void StaticClose( TAny* ptr );

	CLogFileHandler* iLogFileHandler;
	TPtrC            iLogDirectory;	
    TAny*            iObjectAddress;    
	TBool            iCloseBetweenWrites;
    TPtrC8           iClassName8;
    
}; // end class CDebugLog

#endif // _DEBUG_LOG_H_

