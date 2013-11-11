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
* Description:  Container store content manager.
*
*/
#ifndef __CONTAINER_STORE_CONTENT_MANAGER_H__
#define __CONTAINER_STORE_CONTENT_MANAGER_H__



// ========
// INCLUDES
// ========

#include <e32base.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStore.h"

// ====================
// FORWARD DECLARATIONS
// ====================

class CContainerStoreUtils;
class CContainerStoreEncryption;
class CBufferedReader;
class CBufferedWriter;

// =======
// CLASSES
// =======

class CContainerStoreContentManager : public CBase
    {
    public:
    
        static CContainerStoreContentManager* NewL( CContainerStoreUtils&      aUtils,
                                                    CContainerStoreEncryption& aEncryption );
                                                    
        virtual ~CContainerStoreContentManager();          
        
        static void WipeContentFiles( RFs& aFs ); 
    
		TUint ContentLengthL( TContainerId aId, TBool aIsEncrypted );
		
		void ReplaceContentL( TContainerId  aId, 
							  const TDesC8& aContent,
                              TBool         aIsContentEncrypted );


		void ReplaceContentL( TContainerId aId, 
							  RFile&       aContentFile,
                              TBool        aIsContentEncrypted );

		void AppendContentL( TContainerId  aId, 
							 const TDesC8& aContent,
                             TBool         aIsContentEncrypted );

        void PrependContentL( TContainerId  aId, 
                             const TDesC8& aContent,
                             TBool         aIsContentEncrypted );

        void RemoveContentL( TContainerId aId );

		// will truncate the content if the content buffer is too small							
		void FetchContentL( TContainerId aId,					
						    TDes8&      aContent,
                            TBool       aIsContentEncrypted,
						    TUint       aStartPosition = 0 );
						    
		void FetchContentL( TContainerId aId,
							RFile&       aDestinationFile,
                            TBool        aIsContentEncrypted );
        
        void OpenContentFileL( TContainerId aId, 
                               RFs& aFs, 
                               RFile& aFile,
                               TBool aIsContentEncrypted );

							
		void CopyContentL( TContainerId aOldId, 
		                   TContainerId aNewId );
    		                   
        TBool SearchContentL( TContainerId aContainerId, 
                              const TDesC& aSearchString,
                              TDes8&       aSearchBuffer,
                              TBool        aIsContentEncrypted ); 
        
        void EncryptL( TContainerId aId );
        void DecryptL( TContainerId aId );
		
        void StartCopyTransaction();
        void RollbackCopyTransaction();
        
        /**
         * Transfer the ownership of an external content file to the content
         * manager.
         * @param aContentPath path and file name of an existing content file
         */
        void TransferContentFileL(
            TContainerId aId, 
            const TDesC& aContentPath );
        
        
	private:

        CContainerStoreContentManager( CContainerStoreUtils&      aUtils,
                                       CContainerStoreEncryption& aEncryption );
    
        void ConstructL();

        void CleanupTemporaryFile();

        void CreateContentSubdirectoriesL();

		void ContentFilename( TContainerId aId, TDes& aFilename );		        
		
		void CommitTemporaryFileL( TInt aResult, TContainerId aContainerId );
		
        TBool EncryptAndWriteNextBlockL( CBufferedWriter& aBufferedWriter, 
                                         const TDesC8&    aPlaintextBuffer );
                              
        TBool ReadNextBlockL( CBufferedReader& aBufferedReader, 
                              TDes8&           aPlaintextBlock,
                              TBool            aIsContentEncrypted = ETrue );

        void UpdateBlockSizeL(  TBool isEncrypted );
        
        TInt FindVomitCode( TPtr16& aBody );

	private:
        
        CContainerStoreUtils&      iUtils;
        CContainerStoreEncryption& iEncryption;
        RFs&                       iFs;
        TFileName                  iTemporaryFilename; 
        TFileName                  iEncryptDecryptTempFilename;
        TInt                       iBlockSize;        
        //used to store content file ids when copying a message so they can be
        //rolled back if the transaction fails.
        RArray<TContainerId>       iCopiedContentFiles;
    
        __LOG_DECLARATION
        
    }; // end CContainerStoreContentManager
    
#endif
