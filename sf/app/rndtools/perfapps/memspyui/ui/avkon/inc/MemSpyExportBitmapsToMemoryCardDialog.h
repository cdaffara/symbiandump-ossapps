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

#ifndef MEMSPYEXPORTBITMAPSTOMEMORYCARDDIALOG_H
#define MEMSPYEXPORTBITMAPSTOMEMORYCARDDIALOG_H

// System includes
#include <AknProgressDialog.h>
#include <AknWaitDialog.h>
#include <imageconversion.h>

// Classes referenced
class CMemSpyEngineFbServBitmap;
class CMemSpyEngineFbServBitmapArray;



class CMemSpyExportBitmapsToMemoryCardDialog : public CActive, public MProgressDialogCallback
    {
public:
    static CMemSpyExportBitmapsToMemoryCardDialog* NewL( RFs& aFsSession, const CMemSpyEngineFbServBitmapArray& aBitmaps );
    ~CMemSpyExportBitmapsToMemoryCardDialog();

private:
    CMemSpyExportBitmapsToMemoryCardDialog( RFs& aFsSession, const CMemSpyEngineFbServBitmapArray& aBitmaps );
    void ConstructL();

public: // API
    void Cancel();
    static TBool CheckMemoryCardAvailableL( RFs& aFsSession );

private: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private: // From MProgressDialogCallback
    void DialogDismissedL( TInt aButtonId );

private: // Internal methods
    void CompleteSelf( TInt aError );
    void ExportBitmapL( const CMemSpyEngineFbServBitmap& aBitmap );
    void IdentifyBmpMimeTypeL();
    void PruneExistingBitmapFilesL();
    void PrepareFolderNameSpecL();
    TBool TryToExportSingleBitmapL();

private: // Member data
    RFs& iFsSession;
    const CMemSpyEngineFbServBitmapArray& iBitmaps;
    TInt iBitmapIndex;
    TBool iDialogDismissed;
    CEikProgressInfo* iProgressInfo;
    CAknProgressDialog* iProgressDialog;
    CImageEncoder* iEncoder;
    HBufC8* iBmpMimeType;
    HBufC* iFolderName;
    RFileExtensionMIMETypeArray iMimeTypeArray;
    };


#endif
