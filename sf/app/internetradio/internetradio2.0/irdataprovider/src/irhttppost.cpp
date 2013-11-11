/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#include <coemain.h>

#include "irdebug.h"
#include "irhttppost.h"
#include "irsettings.h"
#include "irreportsettings.h"

// Maximum data part size.
const TInt KMaxSubmitSize = 1024;

//----------------------------------------------------------------------------
// CIRHttpPost::NewL()
// First phase constructor
//----------------------------------------------------------------------------
//
CIRHttpPost* CIRHttpPost::NewL( const RHTTPTransaction& aHttpTransaction )
    {
    IRLOG_DEBUG( "CIRHttpPost::NewL - Entering" );
    CIRHttpPost* self = new ( ELeave ) CIRHttpPost( aHttpTransaction );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRHttpPost::NewL - Exiting" );
    return self;
    }

//----------------------------------------------------------------------------
// CIRHttpPost::ConstructL()
// 2nd phase constructor
//----------------------------------------------------------------------------
//
void CIRHttpPost::ConstructL()
    {
    IRLOG_DEBUG( "CIRHttpPost::ConstructL - Entering" );
    iSettings = CIRSettings::OpenL();
    IRLOG_DEBUG( "CIRHttpPost::ConstructL - Exiting" );
    }

//----------------------------------------------------------------------------
// CIRHttpPost::GetDataPartFromFileL()
// Read one data part from browse log file.
//----------------------------------------------------------------------------
//
void CIRHttpPost::GetDataPartFromFileL( TDes8& aDataPart )
    {
    IRLOG_DEBUG( "CIRHttpPost::GetDataPartFromFileL - Entering" );
    
    // Count data size to be read
    TInt offset = KMaxSubmitSize;
    if ( iStartPos + offset > iTotalsize )
        {
        offset = iRemainingLength ;
        }

    // Read data part from file
    User::LeaveIfError( iLogFile.Read( iStartPos, aDataPart, offset ) );
    
    // Count remaining data size
    iStartPos += offset;
    iRemainingLength -= offset;

    IRLOG_DEBUG( "CIRHttpPost::GetDataPartFromFileL - Exiting" );
    }

//----------------------------------------------------------------------------
// CIRHttpPost::CIRHttpPost()
// C++ constructor.
//----------------------------------------------------------------------------
//
CIRHttpPost::CIRHttpPost( const RHTTPTransaction& aHttpTransaction )
    {
    IRLOG_DEBUG( "CIRHttpPost::CIRHttpPost - Entering" );
    iTransaction = aHttpTransaction;
    IRLOG_DEBUG( "CIRHttpPost::CIRHttpPost - Exiting" );
    }

//----------------------------------------------------------------------------
// CIRHttpPost::~CIRHttpPost()
// Destructor.
//----------------------------------------------------------------------------
//
CIRHttpPost::~CIRHttpPost()
    {
    IRLOG_DEBUG( "CIRHttpPost::~CIRHttpPost - Entering" );
    iPartPostBuffer.Close();
    iLogFile.Close();
    if ( iSettings )
        {
        iSettings->Close();
        }
    IRLOG_DEBUG( "CIRHttpPost::~CIRHttpPost - Exit" );
    }

//----------------------------------------------------------------------------
// CIRHttpPost::CloseLogFile ()
// Used to Close the log file LogUsage.gz externally from other sources (bug-fixing)
//----------------------------------------------------------------------------
//
void CIRHttpPost::CloseLogFile()
    {
    IRLOG_DEBUG( "CIRHttpPost::CloseLogFile - Entering" );
    iLogFile.Close();
    IRLOG_DEBUG( "CIRHttpPost::CloseLogFile - Exiting" );
    }

//----------------------------------------------------------------------------
// CIRHttpPost::GetNextDataPart()
// Obtains a data part from the supplier.
//----------------------------------------------------------------------------
//        
TBool CIRHttpPost::GetNextDataPart( TPtrC8& aDataPart )
    {
    IRLOG_DEBUG( "CIRHttpPost::GetNextDataPart - Entering" );
    TBool retVal( EFalse ); // Default: several data parts

    // Get data part from browse log file
    iPartPostBuffer.Delete( 0, iPartPostBuffer.Length() );
    TRAPD( err, GetDataPartFromFileL( iPartPostBuffer ) )
    if ( KErrNone == err )
        {
        // Set data part.
        aDataPart.Set( iPartPostBuffer );
        if ( iRemainingLength <= 0 )
            {
            // Last data part.
            retVal = ETrue;
            iLogFile.Close();
            }
        }
    else
        {
        // Last data part.
        iRemainingLength = 0;
        retVal = ETrue;
        iLogFile.Close();
        }

    IRLOG_DEBUG( "CIRHttpPost::GetNextDataPart - Exiting" );
    return retVal;
    }

//----------------------------------------------------------------------------
// CIRHttpPost::ReleaseData()
// Releases the current data part being held at the data supplier.
//----------------------------------------------------------------------------
//
void CIRHttpPost::ReleaseData()
    {
    IRLOG_DEBUG( "CIRHttpPost::ReleaseData - Entering" );
    if ( iRemainingLength > 0 )
        {
        TRAP_IGNORE( iTransaction.NotifyNewRequestBodyPartL() )
        }
    IRLOG_DEBUG( "CIRHttpPost::ReleaseData - Exiting" );
    }

//----------------------------------------------------------------------------
// CIRHttpPost::OverallDataSize()
// Obtains the overall size of the data being supplied.
//----------------------------------------------------------------------------
//
TInt CIRHttpPost::OverallDataSize()
    {
    IRLOG_DEBUG( "CIRHttpPost::OverallDataSize - Entering" );
    TInt retVal( KErrNotFound );
    if ( iTotalsize > 0 )
        {
        retVal = iTotalsize;
        }
    IRLOG_DEBUG( "CIRHttpPost::OverallDataSize - Exiting" );
    return retVal;
    }

//----------------------------------------------------------------------------
// CIRHttpPost::Reset()
// Resets the data supplier.
//----------------------------------------------------------------------------
//
TInt CIRHttpPost::Reset()
    {
    IRLOG_DEBUG( "CIRHttpPost::Reset - Entering" );
    iStartPos = 0;
    iRemainingLength = iTotalsize;
    IRLOG_DEBUG( "CIRHttpPost::Reset - Exiting" );
    return KErrNone;
    }

//----------------------------------------------------------------------------
// CIRHttpPost::GetFileToPostL()
// Gets browse log file size.
//----------------------------------------------------------------------------
//
void CIRHttpPost::GetFileToPostL(TInt *aFileSize)
    {
    IRLOG_DEBUG( "CIRHttpPost::GetFileToPostL - Entering" );

    TFileName filePath = iSettings->PrivatePath();
    filePath.Append( KGZipLogFileName );
    
    iPartPostBuffer.Close();
    iPartPostBuffer.CreateL( KMaxSubmitSize );

    User::LeaveIfError(
    	iLogFile.Open( CCoeEnv::Static()->FsSession(), filePath, EFileRead ) );
    User::LeaveIfError( iLogFile.Size( iTotalsize ) );
    *aFileSize = iTotalsize;
    
    Reset();

    IRLOG_DEBUG( "CIRHttpPost::GetFileToPostL - Exiting." );
    }
    
//----------------------------------------------------------------------------
// CIRHttpPost::SetTransaction()
// Set the transaction.
//----------------------------------------------------------------------------
//
void CIRHttpPost::SetTransaction( const RHTTPTransaction& aHttpTransaction )
    {
    IRLOG_DEBUG( "CIRHttpPost::SetTransaction - Entering." );
    iTransaction = aHttpTransaction;
    IRLOG_DEBUG( "CIRHttpPost::SetTransaction - Exiting." );
    }

// End of file
