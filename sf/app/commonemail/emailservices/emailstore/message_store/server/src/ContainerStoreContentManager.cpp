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
* Description:  Container store content manager implementation.
*
*/



// ========
// INCLUDES
// ========

#include <bautils.h>
#include <s32mem.h>
#include <e32math.h>
 
#include "ContainerStoreContentManager.h"
#include "ContainerStoreUtils.h"
#include "ContainerStoreEncryption.h"
#include "ContainerStoreDefs.h"
#include "messagestoreutils.h"


// =========
// CONSTANTS
// =========

// The heap buffer size for read operations.
const TUint KReadBufferSize = 4096;

// The heap buffer size for write operations.
const TUint KWriteBufferSize = 4096;

//cmail
// The heap buffer size for non-encrypted operations.
// must be less than or equal to KReadBufferSize and KWriteBufferSize
const TUint KNormalBlockSize = 4096;

//~cmail
// The minimum data size passed into and out of the encryption/decryption functions.  This is also
// the pad length.  This constant will be adjusted to an even multiple of the block size returned
// by the encryption functionality.
const TUint KMinimumBlockSize = 128;


#define KFILE_MODE_READ  (EFileRead | EFileShareAny)
#define KFILE_MODE_WRITE (EFileWrite | EFileShareAny)

// 82187 - Need to exclude the vommit code from the search
_LIT( KSmartForwardTag, ":::" );
_LIT( KSmartForwardTagNoAttach, "::::" );

// =============
// LOCAL CLASSES
// =============

class CBufferedReader : public CBase
    {
    public:
        
        static CBufferedReader* NewLC( RFile& aFile, TInt aBufferSize, TInt aBlockSize );
        
        virtual ~CBufferedReader();
            
        TPtrC8 NextBlockL();
        
        // NextBlockL should be called, and the data consumed, before AtEofL is called.
        TBool AtEofL();
        
    private:

        CBufferedReader( RFile& aFile, TInt aBlockSize );
    
        void ConstructL( TInt aBufferSize );

        void ReadMoreDataL();
            
        RFile&     iFile;
        const TInt iBlockSize;
        TInt       iBufferSize;
        TInt       iBufferPosition;
        RBuf8      iBuffer;        
        
    }; // end class CBufferedReader

class CBufferedWriter : public CBase
    {
    public:
        
        static CBufferedWriter* NewLC( RFile& aFile, TInt aBufferSize, TInt aBlockSize );
            
        virtual ~CBufferedWriter();
        
        TPtr8 NextBlockBufferL();
        
        void TruncateLastBlockL( TInt aTruncationAmount );

        void FlushL();
        
    private:
        
        CBufferedWriter( RFile& aFile, TInt aBlockSize );
    
        void ConstructL( TInt aBlocksToBuffer );
            
        RFile&     iFile;
        const TInt iBlockSize;
        RBuf8      iBuffer;        
    
    }; // end class CBufferedWriter

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CContainerStoreContentManager* CContainerStoreContentManager::NewL( CContainerStoreUtils&      aUtils,
                                                                    CContainerStoreEncryption& aEncryption )
    {
    CContainerStoreContentManager* self = new(ELeave) CContainerStoreContentManager( aUtils, aEncryption );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    } // end NewL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreContentManager::CContainerStoreContentManager( CContainerStoreUtils&      aUtils,
                                                              CContainerStoreEncryption& aEncryption ) :
    iUtils( aUtils ),
    iEncryption( aEncryption ),
    iFs( aUtils.FileSystem() )
    {
    __LOG_CONSTRUCT( "msg", "CContainerStoreContentManager" )
    _LIT( KFormatString, "%ST1" );    
    iTemporaryFilename.Format( KFormatString, &iUtils.PrivatePath() );
    
    _LIT( KEncryptDecryptTempFileFormatString, "%ST2" );    
    iEncryptDecryptTempFilename.Format( KEncryptDecryptTempFileFormatString, &iUtils.PrivatePath() );

    } // end constructor

// ==========================================================================
// FUNCTION: ConstructL
// ==========================================================================
void CContainerStoreContentManager::ConstructL()    
    {
    CleanupTemporaryFile();
    CreateContentSubdirectoriesL();
    } // end ConstructL
    
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreContentManager::~CContainerStoreContentManager()
    {
    __LOG_DESTRUCT
    } // end destructor

// ==========================================================================
// FUNCTION: CleanupTemporaryFile
// ==========================================================================
void CContainerStoreContentManager::WipeContentFiles( RFs& aFs )
    {
    TFileName privatePath;
    TFileName directoryNameWithWildcard;
    
    aFs.PrivatePath( privatePath );

    for( TInt i = 0; i < KNumberOfContentSubdirectories; i++ )
        {
        _LIT( KFormatString, "%SC%i\\*" );
        directoryNameWithWildcard.Format( KFormatString, &privatePath, i );    
        BaflUtils::DeleteFile( aFs, directoryNameWithWildcard );
        } // end for        
    }
    
// ==========================================================================
// FUNCTION: CleanupTemporaryFile
// ==========================================================================
void CContainerStoreContentManager::CleanupTemporaryFile()
    {
	BaflUtils::DeleteFile( iUtils.FileSystem(), iTemporaryFilename );    
    } // end CleanupTemporaryFile
    
// ==========================================================================
// FUNCTION: CreateContentSubdirectoriesL
// ==========================================================================
void CContainerStoreContentManager::CreateContentSubdirectoriesL()
    {
    TFileName directoryName;
    
    for( TInt i = 0; i < KNumberOfContentSubdirectories; i++ )
        {
        _LIT( KFormatString, "%SC%i\\" );
        directoryName.Format( KFormatString, &iUtils.PrivatePath(), i );    
        BaflUtils::EnsurePathExistsL( iUtils.FileSystem(), directoryName );
        } // end for        
    } // end CreateContentSubdirectoriesL
        
// ==========================================================================
// FUNCTION: UpdateBlockSizeL
// ==========================================================================
void CContainerStoreContentManager::UpdateBlockSizeL( TBool isEncrypted )
    {
    TInt actualBlockSize = 0;
    
    if ( isEncrypted )
        {
        actualBlockSize = iEncryption.BlockSizeL();
        
        if( actualBlockSize < KMinimumBlockSize )
            {
            // The minimum block size is already a multiple of the actual block size.
            iBlockSize = KMinimumBlockSize;
            }
        else
            {
            // Round up to the next multiple of the block size.
            iBlockSize = KMinimumBlockSize + actualBlockSize - (KMinimumBlockSize % actualBlockSize);
            } // end if
        }
    else
        {
        iBlockSize = KNormalBlockSize;
        }
    
    } // end UpdateBlockSizeL
        
// ==========================================================================
// FUNCTION: ContentLengthL
// ==========================================================================
TUint CContainerStoreContentManager::ContentLengthL( TContainerId aId, TBool aIsEncrypted )
    {
    TUint returnValue = 0;
    
	TFileName contentFilename;	
	ContentFilename( aId, contentFilename );

    // Open the content file.
	RFile file;
	TInt result = file.Open( iFs, contentFilename, KFILE_MODE_READ );

    if( result == KErrNone )
        {  
        CleanupClosePushL( file );
              
    	TInt fileSize;
    	User::LeaveIfError( file.Size( fileSize ) );
        
        if ( !aIsEncrypted )
            {
            returnValue = fileSize;
            }
        else
            {
            UpdateBlockSizeL( aIsEncrypted );
            
            RBuf8 encryptedBuffer;
            encryptedBuffer.CreateL( iBlockSize );
            CleanupClosePushL( encryptedBuffer );
            
            RBuf8 plaintextBuffer;
            plaintextBuffer.CreateL( iBlockSize );
            CleanupClosePushL( plaintextBuffer );
        	
        	// Read the last block.
    
        	TInt seekPosition = fileSize - iBlockSize;
            User::LeaveIfError( file.Seek( ESeekCurrent, seekPosition ) ); 
        	
            User::LeaveIfError( file.Read( encryptedBuffer, iBlockSize ) );
        	
        	// Decrypt the last block.
            
        	iEncryption.DecryptL( encryptedBuffer, plaintextBuffer );
        	iEncryption.RemovePaddingL( plaintextBuffer );
        	
            // The content length is the file size minus the number of pad bytes in the final block.    	    
            returnValue = fileSize - iBlockSize + plaintextBuffer.Size();
            
            CleanupStack::PopAndDestroy( &plaintextBuffer );        
            CleanupStack::PopAndDestroy( &encryptedBuffer );        
            }
        
        CleanupStack::PopAndDestroy( &file );        
        }
    // If no content file exists then treat it as content length 0.
    else if( result != KErrNotFound )
        {
        User::Leave( result );
        } // end if

    return returnValue;
    
    } // end ContentLengthL

// ==========================================================================
// FUNCTION: CopyContentL
// ==========================================================================
void CContainerStoreContentManager::CopyContentL( TContainerId aOldId, TContainerId aNewId )
	{
	iEncryption.CheckForAuthenticationL();
	
	TFileName oldContentFilename;	
	ContentFilename( aOldId, oldContentFilename );

	RFile file;
	TInt result = file.Open( iFs, oldContentFilename, KFILE_MODE_READ );
	
	if( result == KErrNone )
	    {
    	TInt fileSize;
    	User::LeaveIfError( file.Size( fileSize ) );
    	file.Close();
    	
    	iUtils.LeaveIfLowDiskSpaceL( fileSize );
    	
    	TFileName newContentFilename;	
    	ContentFilename( aNewId, newContentFilename );
        
        // Just copy the content file.  No encryption/decryption is required.
        User::LeaveIfError( BaflUtils::CopyFile( iFs, oldContentFilename, newContentFilename ) );        
        iCopiedContentFiles.AppendL( aNewId );
	    }
    // It is valid not to have a content file, so don't leave in that case.
    else if( result != KErrNotFound )
        {
        User::Leave( result );        
        } // end if
	
	} // end CopyContentL


// ==========================================================================
// FUNCTION: EncryptAndWriteNextBlockL
// ==========================================================================
TBool CContainerStoreContentManager::EncryptAndWriteNextBlockL( CBufferedWriter& aBufferedWriter, 
                                                                const TDesC8&    aPlaintextBlock )
    {
    __LOG_ENTER_SUPPRESS( "EncryptAndWriteNextBlockL" )
       
    TBool lastBlock = EFalse;
    
    TPtr8 nextBlock = aBufferedWriter.NextBlockBufferL();    
    
    if( aPlaintextBlock.Length() == iBlockSize )
        {    
        iEncryption.EncryptL( aPlaintextBlock, nextBlock );
        }
    else
        {
        __LOG_WRITE_INFO( "writing last block" )
        
        lastBlock = ETrue;
        
        // Add PKCS5-stype padding to pad to the block size.
                
        RBuf8 paddedBuffer;
        paddedBuffer.CreateL( iBlockSize );
        CleanupClosePushL( paddedBuffer );
        
        paddedBuffer.Copy( aPlaintextBlock );
        
        iEncryption.AddPaddingL( paddedBuffer, iBlockSize );                       
        
        iEncryption.EncryptL( paddedBuffer, nextBlock );        
        
        CleanupStack::PopAndDestroy( &paddedBuffer );
        } // end if
    
    return lastBlock;        
    
    } // end EncryptAndWriteNextBlockL

// ==========================================================================
// FUNCTION: ReadNextBlockL
// ==========================================================================
TBool CContainerStoreContentManager::ReadNextBlockL( CBufferedReader& aReader,
                                                     TDes8&           aPlaintextBlock,
                                                     TBool            aIsContentEncrypted )
    {    
    TPtrC8 nextBlock = aReader.NextBlockL();
    
    if ( aIsContentEncrypted )
        {
        iEncryption.DecryptL( nextBlock, aPlaintextBlock );    
        }
    else
        {
        aPlaintextBlock.Copy( nextBlock );
        }
        
    // If this is the last block in the file then remove the pad bytes
    TBool done = aReader.AtEofL();
    if( done && aIsContentEncrypted )   
        {
        iEncryption.RemovePaddingL( aPlaintextBlock );
        } // end if
        
    return done;        
          
    } // end ReadNextBlockL

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
void CContainerStoreContentManager::ReplaceContentL( TContainerId  aId, 
                                				     const TDesC8& aContent,
                                                     TBool         aIsContentEncrypted )
    {
    __LOG_ENTER( "ReplaceContentL" )
    
    if( aContent.Length() == 0 )
        {
        TRAP_IGNORE( RemoveContentL( aId ) );
        }
    else
        {                
        UpdateBlockSizeL( aIsContentEncrypted );
           
       	iUtils.LeaveIfLowDiskSpaceL( aContent.Length() + iBlockSize );  // worst case size
     
        // Open the temporary output file.
        RFile outFile;
        User::LeaveIfError( outFile.Replace( iFs, iTemporaryFilename, EFileWrite ) );
        CleanupClosePushL( outFile );
        
        TInt result   = KErrNone;            
        
        if ( aIsContentEncrypted )
            {
            CBufferedWriter* writer = CBufferedWriter::NewLC( outFile, KWriteBufferSize, iBlockSize );
    
            // Write content blocks to the file until there is no more content.  If the content is
            // exactly a multiple of the block size, then add one extra loop with zero length
            // content, to add the expected final pad characters.
            TInt position = 0;
            TBool done    = EFalse;    
            TInt writeLength = 0;
            while( !done && (result == KErrNone) )
                {
                writeLength = iBlockSize;
                if( writeLength > aContent.Length() - position )
                    {
                    writeLength = aContent.Length() - position;
                    } // end if
                
                TRAP( result, done = EncryptAndWriteNextBlockL( *writer, aContent.Mid( position, writeLength ) ) );
                position += writeLength;
                } // end while
            
            writer->FlushL();

            CleanupStack::PopAndDestroy( writer );
            }
        else
            {
            //The content is not encrypted, just do a simple write
            outFile.Write( aContent );
            result = outFile.Flush();
            }
        CleanupStack::PopAndDestroy( &outFile );        
        
        // Commit the temporary file if the result is OK.  Otherwise, clean it up.
        CommitTemporaryFileL( result, aId );
        
        } // end if
        
    __LOG_EXIT        
    } // end ReplaceContentL

// ==========================================================================
// FUNCTION: ReplaceContentL
// ==========================================================================
void CContainerStoreContentManager::ReplaceContentL( TContainerId aId, 
							                         RFile&       aContentFile,
                                                     TBool        aEncryptContent )
    {
    __LOG_ENTER( "ReplaceContentL" )
    
	TInt fileSize;
	User::LeaveIfError( aContentFile.Size( fileSize ) );
	
    TInt  result   = KErrNone;
    
    if( fileSize == 0 )
        {
        TRAP_IGNORE( RemoveContentL( aId ) );
        }
    else
        {                	
        UpdateBlockSizeL( aEncryptContent );
        
       	iUtils.LeaveIfLowDiskSpaceL( fileSize + iBlockSize );  // worst case size

        RFile outFile;
        User::LeaveIfError( outFile.Replace( iFs, iTemporaryFilename, EFileWrite ) );
        CleanupClosePushL( outFile );

        CBufferedReader* reader = CBufferedReader::NewLC( aContentFile, KReadBufferSize, iBlockSize );
        
        if ( aEncryptContent )
            {
        
            CBufferedWriter* writer = CBufferedWriter::NewLC( outFile, KWriteBufferSize, iBlockSize );
    
            TBool done     = EFalse;
            
            // Write content to the file until there is no more content.  If the content is
            // exactly a multiple of the block size, then add one extra loop with zero length
            // content, to add the expected final pad characters.
            while( !done && (result == KErrNone) )
                {
                TRAP( result, TPtrC8 plaintextBuffer = reader->NextBlockL();
                              done = EncryptAndWriteNextBlockL( *writer, plaintextBuffer ) );
                }  // end while
    
            writer->FlushL();
    
            CleanupStack::PopAndDestroy( writer );        
            }
        else
            {
            //encryption not on, so replace the content
            TBool done = EFalse;
            while( !done  )
                {
                TPtrC8 plaintextBuffer = reader->NextBlockL();
                outFile.Write( plaintextBuffer );
                if ( plaintextBuffer.Length() < iBlockSize )
                    {
                    done = ETrue;
                    }
                }  // end while
            }
        
        CleanupStack::PopAndDestroy( reader );        
        CleanupStack::PopAndDestroy( &outFile );
        
        // Commit the temporary file if the result is OK.  Otherwise, clean it up.
        CommitTemporaryFileL( result, aId );
        
        } // end if
    
    __LOG_EXIT
    } // end ReplaceContentL
  
/**
 *
 */
void CContainerStoreContentManager::TransferContentFileL(
    TContainerId aId, 
    const TDesC& aContentPath )
    {
    __LOG_ENTER( "TransferContentFileL" )
    
    TFileName contentFilename;  
    ContentFilename( aId, contentFilename );
    
    User::LeaveIfError( iFs.Replace( aContentPath, contentFilename ) );
    
    __LOG_EXIT
    }

            
// ==========================================================================
// FUNCTION: AppendContentL
// ==========================================================================
void CContainerStoreContentManager::AppendContentL( TContainerId  aId, 
					                                const TDesC8& aContent,
                                                    TBool         aIsContentEncrypted )
    {
    __LOG_ENTER( "AppendContentL" )
    
    if( aContent.Length() > 0 )
        {
    	TFileName contentFilename;	
    	ContentFilename( aId, contentFilename );
	
    	// Open the content file.
        RFile inFile;
        TInt result = inFile.Open( iFs, contentFilename, KFILE_MODE_READ );
        
        if( result == KErrNone )
            {
            CleanupClosePushL( inFile );

        	TInt fileSize;
        	User::LeaveIfError( inFile.Size( fileSize ) );

            UpdateBlockSizeL( aIsContentEncrypted );        

           	iUtils.LeaveIfLowDiskSpaceL( fileSize + aContent.Length() + iBlockSize ); // worst case size

            CleanupStack::PopAndDestroy( &inFile );            
            
            // Copy the existing content file to the temporary content file.    
            User::LeaveIfError( BaflUtils::CopyFile( iFs, contentFilename, iTemporaryFilename ) );
            
            // Open the temporary content file. 
            RFile outFile;
            User::LeaveIfError( outFile.Open( iFs, iTemporaryFilename, EFileRead | EFileWrite ) );
            CleanupClosePushL( outFile );
            
            if ( aIsContentEncrypted )
                {
                RBuf8 plaintextBlock;
                plaintextBlock.CreateL( iBlockSize );
                CleanupClosePushL( plaintextBlock );
    
                // Read the content of the final block (padding removed).
            	TInt seekPosition = fileSize - iBlockSize;
            	User::LeaveIfError( outFile.Seek( ESeekStart, seekPosition ) );
            	
                CBufferedReader* reader = CBufferedReader::NewLC( outFile, iBlockSize, iBlockSize );
    
                ReadNextBlockL( *reader, plaintextBlock );
                
                CleanupStack::PopAndDestroy( reader );   
    
                // Reposition the file pointer at the start of the final block for writing.
            	User::LeaveIfError( outFile.Seek( ESeekStart, seekPosition ) );
    
                CBufferedWriter* writer = CBufferedWriter::NewLC( outFile, KWriteBufferSize, iBlockSize );
                
                // Add the start of the buffer to be appended to the remaining plaintext from the
                // existing file.
                TInt position   = iBlockSize - plaintextBlock.Length();
                if( position > aContent.Length() )
                    {
                    position = aContent.Length();
                    } // end if       
                if ( position > 0 )
                    {
                    plaintextBlock.Append( aContent.Right( position ) );
                    }
                
                TBool done = EFalse;
                
                TRAP( result, done = EncryptAndWriteNextBlockL( *writer, plaintextBlock ) );
    
                // Write the remaining content to the file until there is no more content.  If the
                // content is exactly a multiple of the block size, then add one extra loop with
                // zero length content, to add the expected final pad characters.
                TInt writeLength = 0;
                while( !done && (result == KErrNone) )
                    {
                    writeLength = iBlockSize;
                    if( writeLength > aContent.Length() - position )
                        {
                        writeLength = aContent.Length() - position;
                        } // end if
                
                    TRAP( result, done = EncryptAndWriteNextBlockL( *writer, aContent.Mid( position, writeLength ) ) );
                    position += writeLength;
                    } // end while
                
                writer->FlushL();
    
                CleanupStack::PopAndDestroy( writer );   
                CleanupStack::PopAndDestroy( &plaintextBlock );   
                }
            else
                {
                //content is not encrypted
                TInt seekPosition = 0;
                User::LeaveIfError( outFile.Seek( ESeekEnd, seekPosition ) );
                outFile.Write( aContent );
                User::LeaveIfError( outFile.Flush() );
                }
            CleanupStack::PopAndDestroy( &outFile );   
            
            // Commit the temporary file if the result is OK.  Otherwise, clean it up.
            CommitTemporaryFileL( result, aId );                   
            }
        else if( result == KErrNotFound )    
            {
            ReplaceContentL( aId, aContent, aIsContentEncrypted );
            }
        else
            {
            User::LeaveIfError( result );
            } // end if

        } // end if
    
    __LOG_EXIT    
    } // end AppendContentL


// ==========================================================================
// FUNCTION: PrependContentL
// ==========================================================================
void CContainerStoreContentManager::PrependContentL(
    TContainerId  aId, 
    const TDesC8& aContent,
    TBool aIsContentEncrypted )
    {
    __LOG_ENTER( "PrependContentL" )
    
    if ( aIsContentEncrypted )
        {
        User::Leave( KErrNotSupported );
        }
        
    if ( aContent.Length() > 0 )
        {
        TFileName contentFilename;  
        ContentFilename( aId, contentFilename );
    
        if ( BaflUtils::FileExists( iFs, contentFilename ) )
            {
            TRAPD( err, MessageStoreUtils::PrependBufferAndCopyFileL(
                iFs, iUtils, contentFilename, iTemporaryFilename, aContent ) );
      
            //Commit the temporary file if the result is OK.
            //Otherwise, clean it up.
            CommitTemporaryFileL( err, aId );                   
            }
        else
            {
            ReplaceContentL( aId, aContent, aIsContentEncrypted );
            }
        }
    
    __LOG_EXIT    
    } //PrependContentL


// ==========================================================================
// FUNCTION: RemoveContent
// ==========================================================================
void CContainerStoreContentManager::RemoveContentL( TContainerId aId )
    {   
    __LOG_ENTER( "RemoveContentL" )
     
	TFileName contentFilename;
	ContentFilename( aId, contentFilename );
    
	TInt rc = BaflUtils::DeleteFile( iUtils.FileSystem(), contentFilename );
    if ( rc != KErrNone && rc != KErrNotFound )
        {
        User::Leave( rc );
        }
	
	__LOG_EXIT
    } // end RemoveContent

// ==========================================================================
// FUNCTION: FetchContentL
// ==========================================================================
void CContainerStoreContentManager::FetchContentL( TContainerId aId,					
                                				   TDes8&       aContent,
                                                   TBool        aIsContentEncrypted, 
                                				   TUint        aStartPosition )
    {  
    __LOG_ENTER( "FetchContentL" )
      
	aContent.SetLength( 0 );
	
	TFileName contentFilename;	
	ContentFilename( aId, contentFilename );
	
	// Open the content file.
    RFile inFile;
    TInt result = inFile.Open( iFs, contentFilename, KFILE_MODE_READ );
    
    if( result == KErrNone )
        {
        CleanupClosePushL( inFile );

    	TInt fileSize;
    	User::LeaveIfError( inFile.Size( fileSize ) );
        
        if( aStartPosition < fileSize )
            {
            
            if ( aIsContentEncrypted )
                {
                UpdateBlockSizeL( aIsContentEncrypted );
                
                RBuf8 plaintextBlock;
                plaintextBlock.CreateL( iBlockSize );
                CleanupClosePushL( plaintextBlock );
    
                // Calculate the position of the first block to read.
                TInt seekPosition = aStartPosition - (aStartPosition % iBlockSize);
    
                // Seek to the calculated position.
                User::LeaveIfError( inFile.Seek( ESeekStart, seekPosition ) );            
                
                CBufferedReader* reader = CBufferedReader::NewLC( inFile, KReadBufferSize, iBlockSize );
                
                // Calculate the position within the plaintext buffer to start copying.
                TInt copyStart = aStartPosition - seekPosition;
    
                // Keep reading until the output buffer is full, or until the end of the content file is reached.
                TBool done = EFalse;
                while( (aContent.Length() < aContent.MaxLength()) && !done )
                    {
                    // Read and decrypt the next block from the content file.
                    done = ReadNextBlockL( *reader, plaintextBlock );
    
                    // Copy the data from the plaintext buffer to the output buffer.
                    TInt copyLength = plaintextBlock.Length() - copyStart;                
                    if( aContent.Length() + copyLength > aContent.MaxLength() )
                        {
                        copyLength = aContent.MaxLength() - aContent.Length();
                        } // end if
                    aContent.Append( plaintextBlock.Mid( copyStart, copyLength ) );
    
                    // All copies (except possibly the first one) start at 0.
                    copyStart = 0;                
                                    
                    } // end while
    
                CleanupStack::PopAndDestroy( reader );   
                CleanupStack::PopAndDestroy( &plaintextBlock );   
                }
            else
                {
                //content is not encrypted
                TInt seekPosition = aStartPosition;
                
                // Seek to the calculated position.
                User::LeaveIfError( inFile.Seek( ESeekStart, seekPosition ) );
                inFile.Read( aContent );
                }
            } // end if        

        CleanupStack::PopAndDestroy( &inFile );   
        }
    else if( result != KErrNotFound )
        {
        User::LeaveIfError( result );
        } // end if

    __LOG_EXIT
    } // end FetchContentL
    
// ==========================================================================
// FUNCTION: FetchContentL
// ==========================================================================
void CContainerStoreContentManager::FetchContentL( TContainerId aId,
					                               RFile&       aDestinationFile,
                                                   TBool        aIsContentEncrypted )
    {
    __LOG_ENTER( "FetchContentL" )
    
	TFileName contentFilename;	
	ContentFilename( aId, contentFilename );
    
	// Open the content file.
    RFile inFile;
    TInt result = inFile.Open( iFs, contentFilename, KFILE_MODE_READ );
    
    if( result == KErrNone )
        {
        CleanupClosePushL( inFile );

    	TInt fileSize;
    	User::LeaveIfError( inFile.Size( fileSize ) );

       	iUtils.LeaveIfLowDiskSpaceL( fileSize );  // worst case size
        
        UpdateBlockSizeL( aIsContentEncrypted );
        
        CBufferedReader* reader = CBufferedReader::NewLC( inFile, KReadBufferSize, iBlockSize );
        
        if ( aIsContentEncrypted )
            {
            
            CBufferedWriter* writer = CBufferedWriter::NewLC( aDestinationFile, KWriteBufferSize, iBlockSize );
            
            // Read blocks from the content file and write them to the destination file,
            // until the end of the content file is reached.
            TBool done = EFalse;
            while( !done )
                {
                TPtr8 writeBuffer = writer->NextBlockBufferL();
                
                if( ReadNextBlockL( *reader, writeBuffer ) )
                    {
                    done = ETrue;
                    
                    writer->TruncateLastBlockL( iBlockSize - writeBuffer.Length() );
                    } // end if
    
                } // end while
    
            writer->FlushL();
    
            CleanupStack::PopAndDestroy( writer );   
            }
        else
            {
            //content is not encrypted, just copy the file
            //encryption not on, so just copy the file
            TBool done = EFalse;
            while( !done )
                {
                TPtrC8 buf = reader->NextBlockL();
                aDestinationFile.Write( buf );
                if ( buf.Length() < iBlockSize )
                    {
                    done = ETrue;
                    } // end if
                } // end while
                User::LeaveIfError( aDestinationFile.Flush() );
            }
            
        CleanupStack::PopAndDestroy( reader );   
        CleanupStack::PopAndDestroy( &inFile );   
        }
    else if( result != KErrNotFound )
        {
        User::LeaveIfError( result );
        } // end if

    __LOG_EXIT
    } // end FetchContentL
							
// ==========================================================================
// FUNCTION: OpenContentFileL
// ==========================================================================
void CContainerStoreContentManager::OpenContentFileL( TContainerId aId, RFs& aFs, RFile& aFile, TBool aIsContentEncrypted  )
    {
    __LOG_ENTER( "OpenContentFileL" )
    
    if ( aIsContentEncrypted )
        {
        User::Leave( KErrNotSupported );
        }
    
    TFileName contentFilename;  
    ContentFilename( aId, contentFilename );
    
    TInt rc = aFile.Open( aFs, contentFilename, KFILE_MODE_READ );
    if ( rc != KErrNone )
        {
        __LOG_WRITE8_FORMAT1_ERROR( "Failed to open content file. err=%d", rc)
        User::Leave( rc );
        }
    
    __LOG_EXIT
    }

// ==========================================================================
// FUNCTION: ContentFilename
// ==========================================================================
void CContainerStoreContentManager::ContentFilename( TContainerId aId, TDes& aFilename )
	{
	_LIT( KFormatString, "%SC%i\\%x" );
	
	// Content files are now stored in a folder hierarchy rather than a flat directory,
	// to avoid a single directory with thousands of files.
	//
	// Content filenames are of the form C:\\private\\x\\Cy\\z", where:
	//
	// x = UID of server executable
	// y = a number between 0 and KNumberOfContentSubdirectories-1
	// z = the ID of the container with the type bits masked out (in hex)	
	
	TUint32 filename = aId & ~KContainerTypeMask;
	TUint32 path     = filename % KNumberOfContentSubdirectories;
	aFilename.Format( KFormatString, &iUtils.PrivatePath(), path, filename );
	
	} // end ContentFilename
	
// ==========================================================================
// FUNCTION: CommitTemporaryFileL
// ==========================================================================
void CContainerStoreContentManager::CommitTemporaryFileL( TInt aResult, TContainerId aContainerId )
    {
    if( aResult != KErrNone )
        {
        CleanupTemporaryFile();
        User::Leave( aResult );
        } // end if
    
    TFileName contentFilename;
    ContentFilename( aContainerId, contentFilename );
    
    // Delete the existing file, if necessary.
    TInt rc = BaflUtils::DeleteFile( iUtils.FileSystem(), contentFilename );
    
    if ( rc == KErrInUse )
        {
        RFile inFile;
        User::LeaveIfError( inFile.Open( iFs, iTemporaryFilename, KFILE_MODE_READ ) );
        CleanupClosePushL( inFile );                                                               //+inFile
        
        CBufferedReader* reader = CBufferedReader::NewLC( inFile, KReadBufferSize, iBlockSize );    //+reader
        
        RFile outFile;
        User::LeaveIfError( outFile.Open( iFs, contentFilename, KFILE_MODE_WRITE ) );
        CleanupClosePushL( outFile );                                                              //+outFile
        
        //the client may still have the content file open, in this case, we can not delete it nor can we rename the temp file to it
        //the only thing we can do is to copy the content of the temp file to it.
        TBool done = EFalse;
        while( !done )
            {
            TPtrC8 buf = reader->NextBlockL();
            outFile.Write( buf );
            if ( buf.Length() < iBlockSize )
                {
                done = ETrue;
                } // end if
            } // end while
        User::LeaveIfError( outFile.Flush() );
        
        CleanupStack::PopAndDestroy( &outFile );
        CleanupStack::PopAndDestroy( reader );
        CleanupStack::PopAndDestroy( &inFile );
        }
    else
        {
        // Rename the temporary file to the new name.
        User::LeaveIfError( BaflUtils::RenameFile( iUtils.FileSystem(), iTemporaryFilename, contentFilename ) );    
        }
    } // end CommitTemporaryFileL	        	

// ==========================================================================
// FUNCTION: SearchContentL
// ==========================================================================
TBool CContainerStoreContentManager::SearchContentL( TContainerId aContainerId, 
                                                     const TDesC& aSearchString,
                                                     TDes8&       aSearchBuffer,
                                                     TBool        aIsContentEncrypted )
    {    
    __LOG_ENTER_SUPPRESS( "SearchContentL" )
    
    __LOG_WRITE_FORMAT1_INFO( "id=%x", aContainerId )
    
    TBool found = EFalse;
    
	TFileName contentFilename;	
	ContentFilename( aContainerId, contentFilename );
	
	// Open the content file.
    RFile inFile;
    TInt result = inFile.Open( iFs, contentFilename, KFILE_MODE_READ );
    
    if( result == KErrNone )
        {
        CleanupClosePushL( inFile );

        if ( aIsContentEncrypted )
            {
            UpdateBlockSizeL( aIsContentEncrypted );
            }
        else
            {
            //we can only search half of the size of the buffer at a time
            iBlockSize = KNormalBlockSize / 2;
            }
        
        CBufferedReader* reader = CBufferedReader::NewLC( inFile, KReadBufferSize, iBlockSize );

        TUint16* unicodePointer = reinterpret_cast<TUint16*>(const_cast<TUint8*>(aSearchBuffer.Ptr()));
        TPtr16   unicodeBuffer( unicodePointer, 0, aSearchBuffer.MaxLength() / 2 ); 
        
        RBuf8 plaintextBlock;
        plaintextBlock.CreateL( iBlockSize );
        CleanupClosePushL( plaintextBlock );
        
        const TUint16* sourcePtr16 = reinterpret_cast<const TUint16*>( plaintextBlock.Ptr() );        

        //  Loop until the search string is found or the end of file is reached.            
        TBool done = EFalse;
        while( !found && !done )
            {            
            // Read and decrypt the next block.
            done = ReadNextBlockL( *reader, plaintextBlock, aIsContentEncrypted );

            // Create a 16-bit overlay to the plain text block.
            TPtrC16 sourceBuffer( sourcePtr16, plaintextBlock.Length() / 2 );

            // Convert the 16-bit text to uppercase and copy to the end of the unicode buffer.
            TUint16* destinationPointer = unicodePointer + unicodeBuffer.Length();
            TPtr16   destinationBuffer( destinationPointer, 0, plaintextBlock.Length() ); 

            destinationBuffer.Copy( sourceBuffer );

            unicodeBuffer.SetLength( unicodeBuffer.Length() + destinationBuffer.Length() );            

            // If the end of file has been reached or the unicode buffer is full then search the
            // unicode buffer for the search string.
            if( done || unicodeBuffer.MaxLength() - unicodeBuffer.Length() < iBlockSize )
                {
                
                // 82187 - need to skip the vomit code 
                if ( done )
                	{
                	TInt index = FindVomitCode( unicodeBuffer );
                	if ( index > 0 )
                		{
                		unicodeBuffer.SetLength( index );
                		}
                	}
                
                // Search for the string in the unicode buffer.                
                TInt rc = unicodeBuffer.MatchC( aSearchString );
                if( rc != KErrNotFound )
                    {
                    __LOG_WRITE8_FORMAT1_INFO( "found in %i", aContainerId )
                    found = ETrue;
                    }
                else
                    {
                    // Copy the end of this buffer to the start of the next buffer, in case the search
                    // string spans the boundary of the buffers.
                    unicodeBuffer.Copy( unicodeBuffer.Right(aSearchString.Length()-1) ); 
                    } // end if
                    
                } // end if

            } // end while

        CleanupStack::PopAndDestroy( &plaintextBlock );   
        CleanupStack::PopAndDestroy( reader );   
        CleanupStack::PopAndDestroy( &inFile );   
        } // end if

    __LOG_WRITE_FORMAT1_INFO( "found=%i", found )
    
    return found;
  
    } // end SearchContentL


// ==========================================================================
// FUNCTION: EncryptL
// ==========================================================================
void CContainerStoreContentManager::EncryptL( TContainerId aId )
    {
    RFile file;
    User::LeaveIfError( file.Replace( iFs, iEncryptDecryptTempFilename, EFileWrite ) );
    CleanupClosePushL( file );
    
    //fetch the content that was NOT encrypted
    FetchContentL( aId, file, EFalse );

    //reposition the file to the begining so that it can be read by ReplaceContent
    TInt pos = 0;
    file.Seek( ESeekStart, pos );
    
    //replace and encrypt the content 
    ReplaceContentL( aId, file, ETrue );
    
    CleanupStack::PopAndDestroy( &file );
    
    BaflUtils::DeleteFile( iFs, iEncryptDecryptTempFilename );    
    }

// ==========================================================================
// FUNCTION: DecryptL
// ==========================================================================
void CContainerStoreContentManager::DecryptL( TContainerId aId )
    {
    RFile file;
    User::LeaveIfError( file.Replace( iFs, iEncryptDecryptTempFilename, EFileWrite ) );
    CleanupClosePushL( file );
    
    //fetch the content that was encrypted
    FetchContentL( aId, file, ETrue );
    
    //reposition the file to the begining so that it can be read by ReplaceContent
    TInt pos = 0;
    file.Seek( ESeekStart, pos );
    
    //replace the content with decrypted buffer
    ReplaceContentL( aId, file, EFalse );
    
    CleanupStack::PopAndDestroy( &file );
    
    BaflUtils::DeleteFile( iFs, iEncryptDecryptTempFilename );    
    }


TInt CContainerStoreContentManager::FindVomitCode( TPtr16& aBody )
{
	__LOG_ENTER_SUPPRESS("FindVomitCode");
	//Search backward for the end of the key
	//Ignore whitespace
	TInt idx = aBody.Length()-1;
	while ( idx >= 0 && TChar(aBody[idx]).IsSpace() ) 
		{
		idx--;
		}
	
	int end = idx+1;
	if ( end - KSmartForwardTag().Length() < 0 )
		{
	    return KErrNotFound;
		}
	
	const TDesC16& p = aBody.Mid(end-KSmartForwardTag().Length() );
	
	if ( p.Find(KSmartForwardTag) == KErrNotFound )
		{
		return KErrNotFound; //No key at end of message
		}
		
	// point to the last character before the end tag
	idx = end - KSmartForwardTag().Length() - 1;
	if (idx < 0)
		{
	    return KErrNotFound;
		}

	// : 67030 [USER-10 CRASH]
	// Turns out that the server is allowed to set the attachments
	// list bit map to empty, causing the last part of the key
	// to be 4 colons in a row instead of three.
	// Check if the tail is 4 :
	if (end-KSmartForwardTagNoAttach().Length() >= 0)
	{
		const TDesC16& p2 = aBody.Mid( end-KSmartForwardTagNoAttach().Length() );
		if ( p2.Find( KSmartForwardTagNoAttach ) != KErrNotFound )
		{
			idx--;
		}
	}
	
	// Replace the end tag with something so we can't find it
	TInt repIdx = idx+1;
	for ( TInt i = repIdx ; i < aBody.Length() ; i++ ) 
	{
		if ( aBody[i] == ':') 
			{
			_LIT(KStar,"*");
			aBody.Replace( i, 1, KStar );
			}
		else
			{
			break;
			}
	}
	
	// Now search backward for beginning of key
    //TInt bodyLen = aBody.Length();
	while ( idx >= 0 )
	{
		const TDesC16& tmp = aBody.Mid(idx);
		int loc = tmp.Find( KSmartForwardTag );
		if ( loc != KErrNotFound )
			{
			break;
			}
		idx--;
	}
	
	// Put the colons back in
	for ( TInt i = repIdx ; i < aBody.Length() ; i++ )
	{
		if ( aBody[i] == '*' ) 
			{
			_LIT(KColon,":");
			aBody.Replace( i, 1, KColon);
			}
		else 
			{
			break;
			}
	}
	if ( idx == 0 ) 
		{
		return KErrNotFound;
		}
		
	__LOG_WRITE_FORMAT1_INFO( "Vomit code found! index=%d", idx );
	return idx;
}


/**
 * Must be called before doing multiple calls to the CopyContentL. 
 */
void CContainerStoreContentManager::StartCopyTransaction()
    {
    iCopiedContentFiles.Reset();
    }


/**
 * If a failure is detected deletes the newly copied content files.
 */
void CContainerStoreContentManager::RollbackCopyTransaction()
    {
    TInt count = iCopiedContentFiles.Count();

    TFileName contentFilename;
    for ( TInt i = 0; i < count; i++ )
        {
        ContentFilename( iCopiedContentFiles[i], contentFilename );
        iFs.Delete( contentFilename );
        }
    }


// ---------------
// CBufferedReader
// ---------------

CBufferedReader* CBufferedReader::NewLC( RFile& aFile, TInt aBufferSize, TInt aBlockSize )
    {
    CBufferedReader* self = new(ELeave) CBufferedReader( aFile, aBlockSize );
    CleanupStack::PushL( self );
    self->ConstructL( aBufferSize );
    return self;
    }

CBufferedReader::CBufferedReader( RFile& aFile, TInt aBlockSize ) :
    iFile( aFile ),
    iBlockSize( aBlockSize )
    {
    }

void CBufferedReader::ConstructL( TInt aBufferSize )
    {
    // Make sure the buffer size is an even multiple of the block size.
    iBufferSize = aBufferSize - (aBufferSize % iBlockSize );     
    iBuffer.CreateL( iBufferSize );
    }

CBufferedReader::~CBufferedReader()
    {
    iBuffer.Close();
    }
    
TPtrC8 CBufferedReader::NextBlockL()
    {
    TInt remainingBufferData = iBuffer.Length() - iBufferPosition;
    
    if( remainingBufferData == 0 )
        {
        ReadMoreDataL();
        remainingBufferData = iBuffer.Length(); 
        } // end if
        
    TInt length = iBlockSize;
                                
    if( length > remainingBufferData )
        {
        length = remainingBufferData;
        } // end if
        
    TPtrC8 returnValue = iBuffer.Mid( iBufferPosition, length );
    
    iBufferPosition += length;
    
    return returnValue;
    }

TBool CBufferedReader::AtEofL()
    {
    if( iBufferPosition >= iBuffer.Length() )
        {
        ReadMoreDataL();
        } // end if
    
    return( iBuffer.Length() == 0 );
    }

void CBufferedReader::ReadMoreDataL()
    {
    iBuffer.SetLength( 0 );
    iBufferPosition = 0;
    
    // Read more data.
    User::LeaveIfError( iFile.Read( iBuffer, iBufferSize ) );            
    }                     
            
// ---------------
// CBufferedWriter
// ---------------

CBufferedWriter* CBufferedWriter::NewLC( RFile& aFile, TInt aBufferSize, TInt aBlockSize )
    {
    CBufferedWriter* self = new(ELeave) CBufferedWriter( aFile, aBlockSize );
    CleanupStack::PushL( self );
    self->ConstructL( aBufferSize );
    return self;
    }
    
CBufferedWriter::CBufferedWriter( RFile& aFile, TInt aBlockSize ) :
    iFile( aFile ),
    iBlockSize( aBlockSize )
    {
    }

void CBufferedWriter::ConstructL( TInt aBufferSize )
    {
    // Make sure the buffer size is an even multiple of the block size.
    iBuffer.CreateL( aBufferSize - (aBufferSize % iBlockSize ) );           
    }
    
CBufferedWriter::~CBufferedWriter()
    {
    iBuffer.Close();
    }

TPtr8 CBufferedWriter::NextBlockBufferL()
    {
    if( iBuffer.Length() + iBlockSize > iBuffer.MaxLength() )
        {
        FlushL();
        } // end if
        
    iBuffer.SetLength( iBuffer.Length() + iBlockSize );
    return iBuffer.RightTPtr( iBlockSize );                
    }

void CBufferedWriter::TruncateLastBlockL( TInt aTruncationAmount )
    {
    iBuffer.SetLength( iBuffer.Length() - aTruncationAmount );
    }

void CBufferedWriter::FlushL()
    {
    User::LeaveIfError( iFile.Write( iBuffer ) );
    iBuffer.SetLength( 0 );
    }
