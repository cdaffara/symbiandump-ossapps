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
* Description:   CUniEditorProcessImageOperation class definition.      
*
*/



#ifndef MSG_UNIFIED_EDITOR_PROCESS_IMAGEOPERATION_H
#define MSG_UNIFIED_EDITOR_PROCESS_IMAGEOPERATION_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <msvapi.h>
#include <apmstd.h>

#include <msgunieditorimageprocessor.h>
#include <cmsvattachment.h>
#include <QObject>

// FORWARD DECLARATIONS

class CMsgImageInfo; 
class CMsgImageControl;
class CUniEditorDocument;
class MMsvAttachmentManager;
class CMsvStore;
class HbAction;
// DATA TYPES

// CLASS DECLARATION

enum TUniEditorProcessImageOperationEvent
    {
    EUniEditorProcessImageOperationProcessing, 
    EUniEditorProcessImageOperationComplete,
    EUniEditorProcessImageOperationError,
    // Cancel is called for active operation
    EUniEditorProcessImageOperationCancel
    };

class MUniEditorProcessImageOperationObserver
    {
    public:
    
        /**
        * Notifies Image process operation result and the new file to be used
        */
        virtual void EditorOperationEvent( 
            TUniEditorProcessImageOperationEvent aEvent,
            TFileName& aFileName) = 0;
    };

/**
* CUniEditorProcessImageOperation
*
* @since 3.2
*/
class CUniEditorProcessImageOperation : public QObject,public CActive,
                                        public MUniImageProcessorCallback
    {
    Q_OBJECT
    
    public: // new operations

        /**
        * Factory method
        */
        static CUniEditorProcessImageOperation* NewL( 
            MUniEditorProcessImageOperationObserver &aObserver);

        /**
        * Start image process operation
        *
        * @param aImageInfo
        * 
        */
        void Process( CMsgImageInfo* aImageInfo);

        /**
        * Destructor
        */
        virtual ~CUniEditorProcessImageOperation();

        /**
        * Detaches image info object.
        */
        CMsgImageInfo* DetachImageInfo();


    protected:

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        TInt RunError( TInt aError );
        
        /**
        * From CActive.
        */
        void DoCancel();
        /**
        * From CUniEditorOperation
        */
        void DoCancelCleanup();
        
        /**
        * From MUniImageProcessorCallback
        */
        void ImageProcessingReady( TSize aBitmapSize, 
            TInt aFileSize, 
            TBool aCompressed );
        
    private: // new operations

        /**
        * C++ constructor
        */
        CUniEditorProcessImageOperation( 
            MUniEditorProcessImageOperationObserver &aObserver);

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Returns optimal send image size.
        */
        TSize ImageSendSize() const;

        /**
        * Checks whether some processing is needed for the image.
        * Stores the possible processing method to "iProcessMethod".
        *
        * @return EFalse if aborted by user,
        *         ETrue  otherwise
        */
        TBool CheckNeedToProcess();

        /**
        * Creates empty attachment.
        */
        void CreateEmptyAttachmentL();

        /**
        * Start check step.
        */
        void DoStartCheck();

        /**
        * Start process step.
        */
        void DoStartProcessL();

        /**
        * Start resolve step.
        */
        void DoStartResolveL();

        /**
        * Processing ready.
        */
        void DoReady();

        /**
        * Performs error handling without state change.
        */
        void DoErrorWithoutStateChange();
        
        /**
        * Opens file by file name. If it fails with KErrPermissionDenied,
        * file is searched from message store and opened from there        
        */
        RFile OpenFileForReadingL();
        
        /**
        * CompleteSelf
        */
        void CompleteSelf( TInt aError );
        
        /**
        * Sets operation pending.
        */
        void SetPending();
        
        /**
        * Completes operation
        */
        void CompleteOperation( TInt aError );
        
		/**
		* Check image size
		*/
        void checkLargeImage();
        
    private slots:
    
		/**
     	* This slot is called when large image insertion query dialog is launched.
     	* @param action selected action (yes or no).
     	*/
    	void onDialogLargeImage(HbAction* action);
    
    private: // data

        enum TUniProcessStates
            {
            EUniProcessImgCheck,
            EUniProcessImgProcess,
            EUniProcessImgResolve,
            EUniProcessImgReady,
            EUniProcessImgError
            };

        enum TUniProcessMethods
            {
            EUniProcessImgMethodNone        = 0x0,
            EUniProcessImgMethodScale       = 0x1,
            EUniProcessImgMethodConvert     = 0x2,
            EUniProcessImgMethodCompress    = 0x4
            };

        CMsgImageInfo*              iImageInfo;

        CMsgImageInfo*              iNewImageInfo;
        RFile                       iNewImageFile;
    
        CUniImageProcessor*         iImageProcessor;

        TInt                        iProcessMethod;

        TSize                       iScaleSize;
        TSize                       iScaleSizeResult;
        TDataType                   iTargetType;
        TBool                       iExactImageScaling;
        TInt                        iOperationState;
        MUniEditorProcessImageOperationObserver &iObserver;
        TInt                        iMaxMmsSize;
        TInt                        iMaxImageWidth;
        TInt                        iMaxImageHeight;
        TInt                        iMmsCreationMode;
        RFs                         iFs;
        TBool largeImageQuery;
    };

#endif //MSG_UNIFIED_EDITOR_PROCESS_IMAGEOPERATION_H
