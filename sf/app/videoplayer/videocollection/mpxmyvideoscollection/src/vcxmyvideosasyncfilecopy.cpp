/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Asynchronous file copy, implemented with several small sync block copies.*
*/




// INCLUDE FILES
#include <mpxlog.h>
#include <bautils.h>
#include "vcxmyvideosasyncfilecopy.h"

/**
* One block shouldn't take more than 1 second to copy to keep cancel
* response fast enough. On the otherhand, the deleting the already
* copied data may take several seconds. 
*
* Copying speed tests (from massmemory to memorycard):
*
* Block size       speed        response time
*   6MB        ->  3.20 MB/s    ~2sec    (unacceptable response time, full speed)
*   3MB        ->  3.17 MB/s    ~1sec    (poor response time, almost full speed)
*   1MB        ->  3.16 MB/s    ~0.3sec  (good response time, almost full speed)
* 0.5MB        ->  2.60 MB/s    ~0.19sec (good response time, some speed loss)
*
* -> 1MB is the best.
*
* * Copying speed tests (from memorycard to massmemory):
*
* Block size       speed        response time
*   3MB        ->  5.470 MB/s    0.548sec
*   1MB        ->  4.393 MB/s    0.228sec
*
* 1M gives slightly reduced speed, great response time. 3MB would be better than
* 1M but can't use it since it gives 1sec response time in memorycard writing.
*/
const TInt KBigBufSize    = 1024 * 1024; // 1MB
const TInt KMediumBufSize = 512 * 1024;  // 0.5MB
const TInt KSmallBufSize  = 100 * 1024;  // 0.1MB

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileCopy* CVcxMyVideosAsyncFileCopy::NewL( RFs& aFs )
    {
    CVcxMyVideosAsyncFileCopy* self = new (ELeave) CVcxMyVideosAsyncFileCopy( aFs );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileCopy::~CVcxMyVideosAsyncFileCopy()
    {
    iSourceFile.Close();
    iTargetFile.Close();
    delete iFileBlockBuffer;
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileCopy::CVcxMyVideosAsyncFileCopy( RFs& aFs )
: iFs( aFs )
    {
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileCopy::ConstructL ()
    {
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileCopy::CopyL
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileCopy::CopyL( const TDesC& aSourceFileName,
        const TDesC& aTargetFileName )
    {
    MPX_DEBUG1("CVcxMyVideosAsyncFileCopy::CopyL() start");
    
    if ( iIsCopying )
        {
        User::Leave( KErrNotReady );
        }

    TInt error = iSourceFile.Open( iFs, aSourceFileName, EFileRead | EFileShareReadersOnly );

    User::LeaveIfError( error );

    error = iTargetFile.Replace( iFs, aTargetFileName, EFileWrite | EFileWriteDirectIO
            | EFileShareExclusive );

    if ( error == KErrPathNotFound )
        {
        error = iFs.MkDirAll( aTargetFileName );
        if ( error == KErrNone )
            {
            error = iTargetFile.Create( iFs, aTargetFileName, EFileWrite
                    | EFileWriteDirectIO | EFileShareExclusive );
            }
        }

    User::LeaveIfError( error );
    User::LeaveIfError( iSourceFile.Size( iDataRemaining ) );
    User::LeaveIfError( iTargetFile.SetSize( iDataRemaining ) );

    TInt suggestBlockSize;
    if ( iDataRemaining > KBigBufSize )
        {
        suggestBlockSize = KBigBufSize;
        }
    else
        {
        suggestBlockSize = iDataRemaining;
        }
    AllocateFileBlockBufferL( suggestBlockSize );

    iReadPos   = 0;
    iIsCopying = ETrue;

#ifdef _DEBUG
    iStartTime.UniversalTime();
#endif

    DoNextBlockCopy();

    MPX_DEBUG1("CVcxMyVideosAsyncFileCopy::CopyL() exit");    
    return !iIsCopying;
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileCopy::CopyIsOngoing
// ----------------------------------------------------------------------------
//
TBool CVcxMyVideosAsyncFileCopy::CopyIsOngoing()
    {
    return iIsCopying;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileCopy::DoNextBlockCopy
// ----------------------------------------------------------------------------
//
TInt CVcxMyVideosAsyncFileCopy::DoNextBlockCopy()
    {    
	TInt err = KErrNone;
	TPtr8 fileBufPtr = iFileBlockBuffer->Des();

	TInt bytesToCopy = Min( iDataRemaining, fileBufPtr.MaxSize() );

	err = iSourceFile.Read( iReadPos, fileBufPtr, bytesToCopy );
	if ( err == KErrNone && fileBufPtr.Length() != bytesToCopy )
	    {
		err = KErrCorrupt;
	    }
	if ( err == KErrNone )
	    {
		err = iTargetFile.Write( iReadPos, fileBufPtr, bytesToCopy );
	    }
	if ( err != KErrNone )
		{
		EndCopy( ETrue /* remove target file */);
		return err;
		}

	iReadPos       += bytesToCopy;
	iDataRemaining -= bytesToCopy;

	if ( iDataRemaining == 0 )
		{
		EndCopy( EFalse /* keep target file */);
		}

	return KErrNone;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileCopy::Cancel
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileCopy::Cancel()
    {
    MPX_DEBUG1("CVcxMyVideosAsyncFileCopy::Cancel() start");
    
    if ( iIsCopying )
        {
        EndCopy( ETrue /* remove target file */ );
        }
    MPX_DEBUG1("CVcxMyVideosAsyncFileCopy::Cancel() exit");
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosAsyncFileCopy::AllocateFileBlockBufferL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosAsyncFileCopy::AllocateFileBlockBufferL( TInt aFileSize )
	{
    delete iFileBlockBuffer;
    iFileBlockBuffer = NULL;
    
	TInt big = Min( aFileSize, KBigBufSize );
	iFileBlockBuffer = HBufC8::New( big );
	if ( !iFileBlockBuffer )
	    {
		iFileBlockBuffer = HBufC8::New( KMediumBufSize );
		}
	if ( !iFileBlockBuffer )
	    {
		iFileBlockBuffer = HBufC8::NewL( KSmallBufSize );
		}
	}

void CVcxMyVideosAsyncFileCopy::EndCopy( TBool aRemoveTargetFile )
	{
    MPX_DEBUG1("CVcxMyVideosAsyncFileCopy::EndCopy() start");

    TTime modified;
    iSourceFile.Modified( modified );
    iTargetFile.SetModified( modified );

#ifdef _DEBUG

    TDateTime dT = modified.DateTime();
    TBuf<200> buf;
    buf.Format(_L("CVcxMyVideosAsyncFileCopy:: setting file modified date: %2d.%2d.%4d %2d:%2d:%2d"),
            dT.Day()+1, dT.Month()+1, dT.Year(), dT.Hour(), dT.Minute(), dT.Second() ); 
    MPX_DEBUG2("%S", &buf ); 

    const TInt megaBytes = 0x100000;
    const TInt kiloBytes = 1024;
    TInt dataCopiedInMegaBytes = iReadPos / megaBytes;
    TTime now;
    now.UniversalTime();
    TTimeIntervalSeconds interval;
    now.SecondsFrom( iStartTime, interval );
    TInt copyingSpeed = 0;
    if ( interval.Int() > 0 )
        {
        copyingSpeed = (iReadPos / kiloBytes) / interval.Int();
        }
    MPX_DEBUG3("CVcxMyVideosAsyncFileCopy:: Data copied: %d (MB), %d kB/s", dataCopiedInMegaBytes, copyingSpeed);
#endif
	    
	delete iFileBlockBuffer;
	iFileBlockBuffer = NULL;
	iReadPos       = 0;
	iDataRemaining = 0;
	iSourceFile.Close();
	iIsCopying = EFalse;

	if ( aRemoveTargetFile )
	    {
	    TFileName fileName;
	    TInt err = iTargetFile.FullName( fileName );

        iTargetFile.Close();	
	    
	    if ( err == KErrNone )
	        {
            MPX_DEBUG2("CVcxMyVideosAsyncFileCopy:: removing file: %S", &fileName);
	        iFs.Delete( fileName );
	        }
	    }
	else
	    {
        iTargetFile.Close();
	    }

    MPX_DEBUG1("CVcxMyVideosAsyncFileCopy::EndCopy() exit");
	}
