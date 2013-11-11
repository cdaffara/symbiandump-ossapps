/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "MemSpyExportBitmapsToMemoryCardDialog.h"

// System includes
#include <eikprogi.h>
#include <AknWaitDialog.h>
#include <AknQueryDialog.h>
#include <coemain.h>
#include <eikenv.h>
#include <avkon.hrh>
#include <memspyui.rsg>

// Engine includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperstack.h>
#include <memspy/engine/memspyenginehelperthread.h>
#include <memspy/engine/memspyenginehelperprocess.h>
#include <memspy/engine/memspyenginehelperfbserv.h>

// Driver includes
#include <memspy/driver/memspydriverenumerationsshared.h>

// Literal constants
_LIT( KMemSpyBitmapExportFolderAndFileName, "e:\\MemSpy\\Images\\%04d%02d%02d_%02d.%02d.%02d\\");


CMemSpyExportBitmapsToMemoryCardDialog::CMemSpyExportBitmapsToMemoryCardDialog( RFs& aFsSession, const CMemSpyEngineFbServBitmapArray& aBitmaps )
:   CActive(EPriorityIdle), iFsSession(aFsSession), iBitmaps(aBitmaps)
    {
    CActiveScheduler::Add(this);
    }


CMemSpyExportBitmapsToMemoryCardDialog::~CMemSpyExportBitmapsToMemoryCardDialog()
    {
    Cancel();
    //
    delete iProgressDialog;
    //
    iMimeTypeArray.ResetAndDestroy();
    iMimeTypeArray.Close();
    //
    if ( iEncoder )
        {
        iEncoder->Cancel();
        }
    delete iEncoder;
    delete iFolderName;
    delete iBmpMimeType;
    }


void CMemSpyExportBitmapsToMemoryCardDialog::ConstructL()
    {
    // Check card is ready
    if  ( CheckMemoryCardAvailableL( iFsSession ) == EFalse )
        {
        User::Leave( KErrNotReady );
        }

    // Get mime type for BMP file
    IdentifyBmpMimeTypeL();

    // Prepare unique empty folder
    PrepareFolderNameSpecL();

    // Prepare UI stuff
    iProgressDialog = new( ELeave ) CAknProgressDialog( reinterpret_cast< CEikDialog** >( &iProgressDialog ) );
    iProgressDialog->SetCallback( this );
    iProgressDialog->SetGloballyCapturing( ETrue );
    iProgressDialog->PrepareLC( R_MEMSPY_EXPORT_TO_MEMORYCARD_DIALOG );
    //
    iProgressInfo = iProgressDialog->GetProgressInfoL( );
    iProgressInfo->SetFinalValue( iBitmaps.Count() );
    //
    iProgressDialog->MakeVisible( ETrue ); // Visible right away
    //
    iDialogDismissed = EFalse;
    iProgressInfo->SetAndDraw( 0 );

    // Start the process rolling...
    CompleteSelf(KErrNone);

    //
    iProgressDialog->RunLD( );
    }


CMemSpyExportBitmapsToMemoryCardDialog* CMemSpyExportBitmapsToMemoryCardDialog::NewL( RFs& aFsSession, const CMemSpyEngineFbServBitmapArray& aBitmaps )
    {
    CMemSpyExportBitmapsToMemoryCardDialog* self = new(ELeave) CMemSpyExportBitmapsToMemoryCardDialog( aFsSession, aBitmaps );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyExportBitmapsToMemoryCardDialog::Cancel()
    {
    CActive::Cancel();
    }


TBool CMemSpyExportBitmapsToMemoryCardDialog::CheckMemoryCardAvailableL( RFs& aFsSession )
    {
    TBool available = EFalse;
    //
    TDriveInfo driveInfo;
    if  ( aFsSession.Drive( driveInfo, EDriveE ) == KErrNone )
        {
        // Other checks needed?
        available = ETrue;
        }
    //
    return available;
    }


void CMemSpyExportBitmapsToMemoryCardDialog::RunL()
    {
    const TInt error = iStatus.Int();
    User::LeaveIfError( error );
 
    if  ( !iDialogDismissed )
        {
        // Do we have more items to process?
        if  ( TryToExportSingleBitmapL() )
            {
            _LIT(KTextFormat, "Saving image\n%d/%d");
            TBuf<128> buf;
            buf.Format( KTextFormat, iBitmapIndex, iBitmaps.Count() );
            //
            iProgressDialog->SetTextL( buf );
            iProgressInfo->SetAndDraw( iBitmapIndex );
            iProgressDialog->DrawDeferred();
            }
        else
            {
            // We're done - RunL will not be called again
            if  (iProgressInfo)
                {
                const TInt finalValue = iProgressInfo->Info().iFinalValue;
                iProgressInfo->SetAndDraw( finalValue );
                //
                delete iProgressDialog;
                iProgressDialog = NULL;
                //
                delete this;
                }
            }

        User::ResetInactivityTime();
        }
    else
        {
        delete iProgressDialog;
        iProgressDialog = NULL;
        //
        delete this;
        }
    }


void CMemSpyExportBitmapsToMemoryCardDialog::DoCancel()
    {
    if  ( iEncoder )
        {
        iEncoder->Cancel();
        }

    iDialogDismissed = ETrue;
    }


TInt CMemSpyExportBitmapsToMemoryCardDialog::RunError( TInt /*aError*/ )
    {
    TRAP_IGNORE(

        // Try the next image
        if  ( TryToExportSingleBitmapL() == EFalse )
            {
            iDialogDismissed = ETrue;
            delete this;
            }
    );
    //
    return KErrNone;
    }


void CMemSpyExportBitmapsToMemoryCardDialog::DialogDismissedL( TInt /*aButtonId*/ )
    {
    iDialogDismissed = ETrue;
    iProgressDialog = NULL;
    }


void CMemSpyExportBitmapsToMemoryCardDialog::CompleteSelf( TInt aError )
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );
    SetActive();
    }


void CMemSpyExportBitmapsToMemoryCardDialog::ExportBitmapL( const CMemSpyEngineFbServBitmap& aBitmap )
    {
    // Create a filename
    TFileName fileName( *iFolderName );
    aBitmap.GetExportableFileNameL( fileName );

    // Create encoder object...
    CImageEncoder* encoder = CImageEncoder::FileNewL( iFsSession, fileName, *iBmpMimeType );
    delete iEncoder;
    iEncoder = encoder;

    // Start operation
    iEncoder->Convert( &iStatus, aBitmap.Bitmap() );
    SetActive();
    }


void CMemSpyExportBitmapsToMemoryCardDialog::IdentifyBmpMimeTypeL()
    {
    _LIT(KBmpExtension, ".bmp");
    CImageEncoder::GetFileTypesL( iMimeTypeArray );
    //
    const TInt count = iMimeTypeArray.Count();
    //
    for( TInt i=0; i<count; i++ )
        {
        CFileExtensionMIMEType* extension = iMimeTypeArray[ i ];
        if  ( extension->FileExtension().FindF( KBmpExtension ) >= 0 )
            {
            // Found it - save mime type info
            iBmpMimeType = extension->MIMEType().AllocL();
            iMimeTypeArray.ResetAndDestroy();
            iMimeTypeArray.Close();
            return;
            }
        }
    //
    User::Leave( KErrNotFound );    
    }


void CMemSpyExportBitmapsToMemoryCardDialog::PrepareFolderNameSpecL()
    {
    delete iFolderName;
    iFolderName = NULL;
    
    // Make a unique filename format specifier.
    TTime now;
    now.HomeTime();
    const TDateTime dt( now.DateTime() );

    // Build it up...
    iFolderName = HBufC::NewL( KMaxFileName );
    TPtr pName( iFolderName->Des() );
    pName.Format( KMemSpyBitmapExportFolderAndFileName, dt.Year(), dt.Month()+1, dt.Day()+1, dt.Hour(), dt.Minute(), dt.Second());
    
    // Create directory
    const TInt error = iFsSession.MkDirAll( pName );
    User::LeaveIfError( error );
    }


TBool CMemSpyExportBitmapsToMemoryCardDialog::TryToExportSingleBitmapL()
    {
    TBool startedExport = EFalse;
    
    const TInt count = iBitmaps.Count();
    for( ; ( iBitmapIndex < count ) && ( startedExport == EFalse ); iBitmapIndex++ )
        {
        const CMemSpyEngineFbServBitmap& memSpyBitmap = iBitmaps.At( iBitmapIndex );
        const CFbsBitmap& fbsBitmap = memSpyBitmap.Bitmap();
        const TSize size( fbsBitmap.SizeInPixels() );

        // We don't want to export bitmaps which have a zero-pixel dimension.
        if  ( size.iHeight > 0 && size.iWidth > 0 )
            {
            // This completes the object's request status and will therefore
            // result in a further call to RunL() when the export to BMP
            // is completed.
            ExportBitmapL( memSpyBitmap );

            // And indicate we managed to start an operation...
            startedExport = ETrue;
            }
        }
    //
    return startedExport;
    }












