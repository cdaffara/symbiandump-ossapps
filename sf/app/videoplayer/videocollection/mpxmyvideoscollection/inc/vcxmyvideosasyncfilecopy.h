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




#ifndef VCXMYVIDEOSASYNCFILECOPY_H
#define VCXMYVIDEOSASYNCFILECOPY_H

// INCLUDES

// FORWARD DECLARATIONS

// CONSTANTS

// CLASS DECLARATION

/**
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosAsyncFileCopy) :
    public CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @return object constructed
        */
        static CVcxMyVideosAsyncFileCopy* NewL( RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosAsyncFileCopy();

    public:
        /**
        * Copies aSourceFileName to aSourceFileName. Leaves with KErrNotReady
        * if there is already copy operation going on.
        *
        * @param aSourceFileName  Full path and name of the source file.
        * @param aTargetFileName  Full path and name of the target file.
        * @return                 ETrue if copy was finished. If EFalse, then
        *                         user must do DoNextBlockCopyL calls to
        *                         continue copying.
        */
        TBool CopyL( const TDesC& aSourceFileName, const TDesC& aTargetFileName );
        
        /**
        * Checks if there is copy going on.
        *
        * @return ETrue if copy is going on, EFalse otherwise.
        */
        TBool CopyIsOngoing();
        
        /**
        * Copies next block from the current copy operation.
        *
        * @return System wide error code.
        */
        TInt DoNextBlockCopy();

        /**
        * Cancels current copy operation (if any). Does nothing if copy
        * operation is not going on. Cancel removes the possible target file
        * which has been created during the current copy operation.
        */
        void Cancel();
        
    private:
        /**
        * Constructor
        */
        CVcxMyVideosAsyncFileCopy( RFs& aFs );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();
        
        /**
        * Allocates buffer from RAM (iFileBlockBuffer) for a file block.
        * 
        * @param aFileSize  Size of the file to be copied.
        */
        void AllocateFileBlockBufferL( TInt aFileSize );
        
        /**
        * Closes file handles and resets variables.
        *
        * @param aRemoveTargetFile If ETrue then the target file is removed.
        */
        void EndCopy( TBool aRemoveTargetFile );
                
    private:

        /**
        * File server session.
        */
        RFs& iFs;

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

        /**
        * File in which we are copying from.
        */ 
        RFile64 iSourceFile;
        
        /**
        * File which is being copied to.
        */ 
        RFile64 iTargetFile;

        /**
        * How much data is left for copying.
        */
        TInt64 iDataRemaining;
        
        /**
        * Current read position in source file. 
        */
        TInt64 iReadPos;                      
#else

        /**
        * File in which we are copying from.
        */ 
        RFile iSourceFile;
        
        /**
        * File which is being copied to.
        */ 
        RFile iTargetFile;

        /**
        * How much data is left for copying.
        */
        TInt iDataRemaining;
        
        /**
        * Current read position in source file. 
        */
        TInt iReadPos;                      

#endif
        /**
        * Set to ETrue when copy starts and EFalse when copying ends.
        */
        TBool iIsCopying;
        
        /**
        * Space for the one file block. Own
        */
        HBufC8* iFileBlockBuffer;

#ifdef _DEBUG
        /**
        * Used to calculate copying speed.
        */
        TTime iStartTime;
#endif        
    };

#endif   // VCXMYVIDEOSASYNCFILECOPY_H


