/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides CUniEditorProcessImageOperation methods.
*
*/



// ========== INCLUDE FILES ================================

#include <basched.h>
#include <icl/imagedata.h>

#include <centralrepository.h>

#include <MessagingVariant.hrh>
#include <MessagingInternalCRKeys.h> // for Central Repository keys

#include <MsgMediaResolver.h>
#include <MsgImageInfo.h>
#include <MmsConformance.h>
#include <MsgMimeTypes.h>

#include <msgunieditorimageprocessor.h>


#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cmsvattachment.h>

#include <msgtextutils.h>

#include <msvids.h>
#include <MmsEngineDomainCRKeys.h>
#include <msgunieditormmssettingsdefs.h>
#include <HbMessageBox>
#include <HbAction>
#include <mmsconst.h>

#include "msgunieditormonitor.h"
#include "msgunieditorprocessimageoperation.h"

// ========== CONSTANTS ====================================

// Leave some space after compression so that text can be inserted to the 
// message.
const TInt KUniCompressionMargin = 10 * 1024; // 10 kB

//const TInt KUniMmsUploadImageWidth = 1600;
//const TInt KUniMmsUploadImageHeight = 1200;


_LIT8( KUniExtImageJpeg_8, ".jpg" );
_LIT8( KUniExtImageGif_8,  ".gif" );

_LIT(KTempFilePath,"c:\\temp\\unieditor\\");

#define LOC_LARGE_IMAGE_NOTE QString("The receiver may not support image this large. Continue?")


// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::NewL
//
// Factory method.
// ---------------------------------------------------------
//
CUniEditorProcessImageOperation* CUniEditorProcessImageOperation::NewL(
    MUniEditorProcessImageOperationObserver &aObserver)
    {
    CUniEditorProcessImageOperation* self = new ( ELeave ) 
                    CUniEditorProcessImageOperation(aObserver);
            
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CUniEditorProcessImageOperation.
// ---------------------------------------------------------
//
CUniEditorProcessImageOperation::CUniEditorProcessImageOperation(
    MUniEditorProcessImageOperationObserver &aObserver) 
    :    CActive( EPriorityStandard ),
         iObserver(aObserver)
    {
            CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::ConstructL
//
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::ConstructL()
    {
    User::LeaveIfError(iFs.Connect());
    iFs.ShareProtected();            

    TInt featureBitmask( 0 );
    
    CRepository* repository = CRepository::NewL( KCRUidMuiuVariation );
    repository->Get( KMuiuMmsFeatures, featureBitmask );
    
    iExactImageScaling = ( featureBitmask & KMmsFeatureIdExactImageScaling );
    
    delete repository;
    
    // MMS Engine CR
    repository = CRepository::NewL( KCRUidMmsEngine );


    TInt temp = 0;
    if ( repository->Get( KMmsEngineImageWidth, temp ) == KErrNone )
    {
        iMaxImageWidth = temp;
    }
    temp = 0;
    if ( repository->Get( KMmsEngineImageHeight, temp ) == KErrNone )
    {
        iMaxImageHeight = temp;
    }

    // Just to make sure the size is at least "small"
    iMaxImageWidth = Max( KMmsUniImageSmallWidth, iMaxImageWidth );
    iMaxImageHeight = Max( KMmsUniImageSmallHeight, iMaxImageHeight );
    
    temp = 0;
    if ( repository->Get( KMmsEngineCreationMode, temp ) == KErrNone )
    {
        iMmsCreationMode = temp;
    }
    
    delete repository;

    iMaxMmsSize = MsgUnifiedEditorMonitor::maxMmsSize();
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::~CUniEditorProcessImageOperation
// ---------------------------------------------------------
//
CUniEditorProcessImageOperation::~CUniEditorProcessImageOperation()
    {
    Cancel();    
    delete iNewImageInfo;
    iNewImageInfo = NULL;
    iNewImageFile.Close();
    delete iImageProcessor;

    //Since iFs doesnot have recursive dir deletion use file manager
    TRAP_IGNORE(
        CFileMan *fm = CFileMan::NewL(iFs);
        fm->RmDir(KTempFilePath);
        delete fm;
        );

    iFs.Close();
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::Process
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::Process( CMsgImageInfo* aImageInfo )
    {    
    iImageInfo = aImageInfo;
    iOperationState = EUniProcessImgCheck;
    
    CompleteSelf( KErrNone );
    }



// ---------------------------------------------------------
// CUniEditorProcessImageOperation::RunL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::RunL()
    {
    if ( iStatus.Int() != KErrNone )
        {        
        iOperationState = EUniProcessImgError;
        }
    TFileName newFileName;
    
    switch ( iOperationState )
        {
        case EUniProcessImgCheck:
            {
            DoStartCheck();
            break;
            }
        case EUniProcessImgProcess:
            {
            DoStartProcessL();
            break;
            }
        case EUniProcessImgResolve:
            {
            DoStartResolveL();
            break;
            }
        case EUniProcessImgReady:
            {
            if(iNewImageInfo) 
            {    
            newFileName.Append(iNewImageInfo->FullFilePath());
            }
            iObserver.EditorOperationEvent( EUniEditorProcessImageOperationComplete,newFileName );
            break;
            }
        case EUniProcessImgError:
            {
            DoErrorWithoutStateChange();            
            iObserver.EditorOperationEvent( EUniEditorProcessImageOperationError,newFileName );
            break;
            }
        default:
            {
            iObserver.EditorOperationEvent( EUniEditorProcessImageOperationError,newFileName );
            break;
            }
        }    
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoCancelCleanup
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoCancelCleanup()
    {
    if ( iImageProcessor )
        {
        iImageProcessor->Cancel();
        }
        
    DoErrorWithoutStateChange();
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoStartCheck
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoStartCheck()
    {
    if ( !CheckNeedToProcess() )
        {
        iOperationState = EUniProcessImgError;
        }
    else if ( iProcessMethod == EUniProcessImgMethodNone )
        {
        //Since resizing is not started 
        //we should ensure that original file is picked for insertion
        iOperationState = EUniProcessImgError; 
        }
    else
        {
        iOperationState = EUniProcessImgProcess;
        }
    checkLargeImage();
    }
  
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoStartProcessL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoStartProcessL()
    {
    CreateEmptyAttachmentL();
    
    if ( !iImageProcessor )
        {
        iImageProcessor = new( ELeave )CUniImageProcessor( this );
        }
        
    RFile sourceFile = OpenFileForReadingL();
    
    CleanupClosePushL( sourceFile );
    
    iImageProcessor->ProcessImageL( sourceFile,
                                    iNewImageFile,
                                    iScaleSize,
                                    iTargetType.Des8(),
                                    ETrue, // Always maintain aspect ratio
                                    iMaxMmsSize - KUniCompressionMargin );
    SetPending();
                   
    CleanupStack::PopAndDestroy(); // sourceFile;
    }
  
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoStartResolveL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoStartResolveL()
    {
    CMsgMediaResolver* mediaResolver = CMsgMediaResolver::NewL();
    mediaResolver->SetCharacterSetRecognition( EFalse );    
    CleanupStack::PushL(mediaResolver);
    
    //Delete the previous object if present
    delete iNewImageInfo;
    iNewImageInfo = NULL;
    iNewImageInfo = static_cast<CMsgImageInfo*>(mediaResolver->CreateMediaInfoL( iNewImageFile ) );
    
    mediaResolver->ParseInfoDetailsL( iNewImageInfo, iNewImageFile );

    iOperationState = EUniProcessImgReady;

    iNewImageFile.Close();
    
    CleanupStack::PopAndDestroy(mediaResolver);
    CompleteSelf( KErrNone );
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoErrorWithoutStateChange
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoErrorWithoutStateChange()
    {
    iNewImageFile.Close();
    delete iNewImageInfo;
    iNewImageInfo = NULL;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::ImageProcessingReady
//
// Image Compressor callback implementation.
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::ImageProcessingReady( TSize aBitmapSize, 
    TInt aFileSize, 
    TBool aCompressed )
    {
    TInt err = iImageProcessor->Error();
    
    if ( err == KErrNone &&
         ( aBitmapSize.iWidth == 0 || aBitmapSize.iHeight == 0 ||
           ( aCompressed &&
            ( aFileSize == 0 ||
                    aFileSize > ( iMaxMmsSize - KUniCompressionMargin ) ) ) ) )
        {
        err = KErrGeneral;
        }
        
    switch ( err )
        {
        case KErrNone:
            {
            iOperationState = EUniProcessImgResolve;
            break;
            }
        case KErrNoMemory:
            {
            iOperationState = EUniProcessImgError;

            break;
            }
        case KErrDiskFull:
            {
            iOperationState = EUniProcessImgError;

            break;
            }
        case KErrNotFound:
            {
            iOperationState = EUniProcessImgError;

            break;
            }
        default:
            {
            iOperationState = EUniProcessImgError;
            break;
            }
        }
    
    if ( err == KErrCancel )
        {
        CompleteOperation( KErrCancel );
        }
    else
        {
        CompleteOperation( KErrNone );
        }
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CheckNeedToProcess
// 
// Checks if scaling/converting/compression is needed.
// ---------------------------------------------------------
//
TBool CUniEditorProcessImageOperation::CheckNeedToProcess()
    {
    iProcessMethod = EUniProcessImgMethodNone;
    
    CMmsConformance* mmsConformance = NULL;
    TRAP_IGNORE(mmsConformance = CMmsConformance::NewL());
    TMmsConformance conformance;
    if(mmsConformance)
    {
        mmsConformance->CheckCharacterSet( EFalse );
        conformance = mmsConformance->MediaConformance( *iImageInfo );
    }
    
    if ( conformance.iCanAdapt == EFalse )
        {
        return EFalse;
        }
        
    TSize origSize = iImageInfo->Dimensions();
    
    if ( origSize.iWidth == 0 || origSize.iHeight == 0 )
        {    
        // Cannot get size -> corrupted
        return EFalse; // Abort
        }
    
    TSize scaleSize = ImageSendSize();
    TSize optimizedSize = origSize;
    
    while ( optimizedSize.iWidth > scaleSize.iWidth ||
            optimizedSize.iHeight > scaleSize.iHeight )
        {
        // Largest possible (1/2)^n size
        optimizedSize.iWidth >>= 1;
        optimizedSize.iHeight >>= 1;
        }
        
    if ( scaleSize.iWidth < origSize.iWidth ||
         scaleSize.iHeight < origSize.iHeight )
        {
        if ( !iExactImageScaling &&
            ( scaleSize.iWidth > KMmsUniImageSmallWidth ||
              scaleSize.iHeight > KMmsUniImageSmallHeight ) )
            {
            // Use optimized (1/2^n) size when scaling
            // to larger than "Small"
            scaleSize = optimizedSize;
            }
        // else -> scale to exact (non-optimized) size
        
        iProcessMethod |= EUniProcessImgMethodScale;
        }
    else
        {
        // Scaling not needed. Check possible conversion need.
        scaleSize = origSize;
        
        if ( conformance.iConfStatus & EMmsConfNokConversionNeeded )
            {
            // Conversion needed.
            iProcessMethod |= EUniProcessImgMethodConvert;
            }
        }

    if ( !( iProcessMethod & EUniProcessImgMethodScale ) &&
         ( iImageInfo->FileSize() + 
           MsgUnifiedEditorMonitor::messageSize() ) > iMaxMmsSize &&
         iImageInfo->MimeType().CompareF( KMsgMimeImageJpeg ) == 0 &&
         (MsgUnifiedEditorMonitor::messageSize()) < KUniCompressionMargin )
        {
        // Only compression needed as image is JPEG that is larger than can be fitted
        // into the message and scaling is not performed. Also current message size
        // is under comression margin.
        iProcessMethod |= EUniProcessImgMethodCompress;
        }
    
    largeImageQuery = EFalse;
    
    if ( iProcessMethod == EUniProcessImgMethodNone )
        {
        // Image won't be processed
        if ( ( origSize.iWidth > KImageRichWidth ||
               origSize.iHeight > KImageRichHeight ) &&
             ( iImageInfo->FileSize() + MsgUnifiedEditorMonitor::messageSize() ) < iMaxMmsSize )
            {
            // Original image width or height is "non-conformant" and original image would 
            // fit to into the message without any processing.
            largeImageQuery = ETrue;
            }
        }
    else
        {
        // Image will be processed
        if ( scaleSize.iWidth > KImageRichWidth ||
             scaleSize.iHeight > KImageRichHeight )
            {
            // Processed image is "non-conformant" after processing.
            largeImageQuery = ETrue;
            }
        }
 
    iScaleSize = scaleSize;
    return ETrue;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::checkLargeImage
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::checkLargeImage()
{
    //Large image query     
    if( largeImageQuery && iMmsCreationMode == EMmsCreationModeWarning)
    {
        HbMessageBox::question(LOC_LARGE_IMAGE_NOTE, this,
                               SLOT(onDialogLargeImage(HbAction*)),
                               HbMessageBox::Yes | HbMessageBox::No); 
    }
    else
    {
        CompleteSelf(KErrNone);
    }
        
}
       

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CreateEmptyAttachmentL
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::CreateEmptyAttachmentL()
    {

    // Get the file name from original full path name.
    TParsePtrC parser( iImageInfo->FullFilePath() );

    TFileName ext( parser.Ext() );
    iTargetType = iImageInfo->MimeType();

    if ( iTargetType.Des8().CompareF( KMsgMimeImagePng ) == 0 )
        {
        //png is non-conformant image type
        //->convert to jpeg
        iTargetType = TDataType( KMsgMimeImageJpeg );
        ext.Zero();
        ext.Copy( KUniExtImageJpeg_8 );
        }
    else if ( iTargetType.Des8().CompareF( KMsgMimeImageWbmp ) == 0 )
        {
        //no wbmp encoder
        //->convert to gif if scaling is needed
        iTargetType = TDataType( KMsgMimeImageGif );
        ext.Zero();
        ext.Copy( KUniExtImageGif_8 );
        }

    //Create dir if doesnot exist, if dir already exits error is returned 
    //which can be ignored
    iFs.MkDirAll(KTempFilePath);
    
    TFileName newFileName( KTempFilePath );
    newFileName.Append(parser.Name());
    newFileName.Append( ext );
    

    iNewImageFile.Replace(iFs,newFileName,EFileWrite | EFileShareAny);

    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::ImageSendSize
// ---------------------------------------------------------
//
TSize CUniEditorProcessImageOperation::ImageSendSize() const
    {
    TSize size;
    size.iWidth = iMaxImageWidth;
    size.iHeight = iMaxImageHeight;
    return size;
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::OpenFileForReadingL
// ---------------------------------------------------------
//
RFile CUniEditorProcessImageOperation::OpenFileForReadingL()
    {
    RFile sourceFile;
    TInt err = sourceFile.Open(
        iFs, iImageInfo->FullFilePath(), EFileRead | EFileShareAny );
    if ( err )
    {
        err = sourceFile.Open(
            iFs, iImageInfo->FullFilePath(), EFileRead | EFileShareReadersOnly );

        User::LeaveIfError( err );
    }
    return sourceFile;
    }
    
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::RunError
// ---------------------------------------------------------
//
TInt CUniEditorProcessImageOperation::RunError( TInt aError )
    {
    delete iImageProcessor;
    iImageProcessor = NULL;    
    
    if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }
    else
        {
        CompleteSelf( aError );
        return KErrNone;
        }
    }
    
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DetachImageInfo
// ---------------------------------------------------------
//
CMsgImageInfo* CUniEditorProcessImageOperation::DetachImageInfo()
    {
    // ownership transferred
    CMsgImageInfo* tempInfo = iNewImageInfo;
    iNewImageInfo = NULL;
    return tempInfo;
    }
    
// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CompleteSelf
//
// Completes current step of state machine
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::CompleteSelf( TInt aError )
    {
    iStatus = KRequestPending;
    TRequestStatus* pStatus = &iStatus;
    SetActive();
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::SetPending
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::SetPending()
    {
    if ( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        }
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::CompleteOperation
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::CompleteOperation( TInt aError )
    {
    SetPending();
        
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aError );
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::DoCancel
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::DoCancel()
    {
    DoCancelCleanup();
    
    
    if ( iStatus == KRequestPending )
        {
        CompleteOperation( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CUniEditorProcessImageOperation::onDialogLargeImage
// ---------------------------------------------------------
//
void CUniEditorProcessImageOperation::onDialogLargeImage(HbAction* action)
    {
    HbMessageBox *dlg = qobject_cast<HbMessageBox*> (sender());
    if (action == dlg->actions().at(1)) {
        iOperationState = EUniProcessImgError;
        }
    CompleteSelf(KErrNone);
    }

// End of file
