/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCntParserInfoLog implementation.
*
*/


#include <s32strm.h>
#include <f32file.h>
#include <s32file.h>

#include "cntparserinfolog.h"

_LIT8(KNewLine,"\r\n");

_LIT8(KMaxRecords,"Maximum-Records:");
_LIT8(KTotalRecords,"Total-Records:");

//------------------------------------------------------------
// CCntParserInfoLog::CCntParserInfoLog()
//------------------------------------------------------------
CCntParserInfoLog::CCntParserInfoLog()
    {
    }

//------------------------------------------------------------
// CCntParserInfoLog::~CCntParserInfoLog()
//------------------------------------------------------------
CCntParserInfoLog::~CCntParserInfoLog()
    {
    }

//------------------------------------------------------------
// CCntParserInfoLog::CCntParserInfoLog::NewL()
//------------------------------------------------------------
EXPORT_C CCntParserInfoLog* CCntParserInfoLog::NewL()
    {
    CCntParserInfoLog* self=new (ELeave) CCntParserInfoLog();
    return self;
    }

//------------------------------------------------------------
// CCntParserInfoLog::SaveToFileL(const TDesC& aFileName) const
//------------------------------------------------------------
EXPORT_C void CCntParserInfoLog::SaveToFileL( const TDesC& aFileName ) const
    {
    TInt err = KErrNone;
    RFs Fs;
    RFile writer;
    User::LeaveIfError( Fs.Connect() ); //Connect to the file server
    CleanupClosePushL( Fs );
    err = writer.Replace( Fs, aFileName, EFileWrite ); //Open file
    CleanupClosePushL( writer );
    
    if( err == KErrNone )
        {
        writer.Write( KMaxRecords );
        
        TBuf8<16> numVal;
        numVal.Num( iMaximumRecords );
        writer.Write( numVal );
        
        writer.Write( KNewLine );
        
        writer.Write( KTotalRecords );
        
        TBuf8<16> numVal2;
        numVal2.Num( iTotalRecords );
        writer.Write( numVal2 );
        }
    
    CleanupStack::PopAndDestroy();  //writer
    CleanupStack::PopAndDestroy();  //Fs
    }

//------------------------------------------------------------
// CCntParserInfoLog::SetTotalRecords(TInt aTotalRecords)
//------------------------------------------------------------
EXPORT_C void CCntParserInfoLog::SetTotalRecords(TInt aTotalRecords)
    {
    iTotalRecords = aTotalRecords;
    }

//------------------------------------------------------------
// CCntParserInfoLog::SetMaximumRecords(TInt aMaximumRecords)
//------------------------------------------------------------
EXPORT_C void CCntParserInfoLog::SetMaximumRecords(TInt aMaximumRecords)
    {
    iMaximumRecords = aMaximumRecords;
    }

//------------------------------------------------------------
// CCntParserInfoLog::TotalRecords() const
//------------------------------------------------------------
EXPORT_C TInt CCntParserInfoLog::TotalRecords() const
    {
    return iTotalRecords;
    }

//------------------------------------------------------------
// CCntParserInfoLog::MaximumRecords() const
//------------------------------------------------------------
EXPORT_C TInt CCntParserInfoLog::MaximumRecords() const
    {
    return iMaximumRecords;
    }
    
// end of file
