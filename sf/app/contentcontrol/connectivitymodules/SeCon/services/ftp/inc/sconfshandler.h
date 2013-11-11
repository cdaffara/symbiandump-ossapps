/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConFSHandler header file
*
*/


#ifndef _SCONFSHANDLER_H_
#define _SCONFSHANDLER_H_

// INCLUDES

#include <e32base.h>
#include <obexobjects.h>


// FORWARD DECLARATIONS
class CDirectoryLocalizer;


//============================================================
// Class CSConFsHandler declaration
//============================================================
NONSHARABLE_CLASS( CSConFsHandler ) : public CBase
    {
    public:
        static CSConFsHandler* NewL( RFs& aFs );
        ~CSConFsHandler();
        /**
         * Parses folder and file listing from specified directory
         * @param aBuffer Buffer to store the folder list
         * @param aPathName Folder path
         * @param aMemNumber memory number (1= DEV or MMC, 2= DEV2 or MMC2..)
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt ParseFolderListL( CBufFlat* aBuffer, const TPtrC aPathName,
            const TInt aMemNumber );
        /**
         * Gets a file object
         * @param aBuffer CObexFileObject object where the file will be stored
         * @param aPathName The path of the file
         * @param aFileName The name of the file
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt GetFileObjectL( CObexFileObject*& aBuffer, const TPtrC aPathName,
            const TPtrC aFileName );
        /**
         * Stores a file object
         * @param aFullPathName The full path and name of the file
         * @param aTime The time stamp of the file
         * @param aTempFileName Data temp file
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt SaveFileObjectL( const TPtrC aFullPathName, const TTime aTime, 
            const TPtrC aTempFileName );
        /**
         * Validates the file name
         * @param aName File name to be validated
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt ValidateFileName( const TPtrC aName );
        /**
         * Creates a folder
         * @param aFolderPath The name and the path of the folder
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt CreateFolderL( const TPtrC aFolderPath );
        /**
         * Removes a folder or a file
         * @param aFullPath The path and name of the object
         * @return KErrNone if no errors, else system wide error codes.
         */
        TInt DeleteObjectL( const TPtrC aFullPath );
        
        /**
         * Copy or Move file/folder
         * @param aSource
         * @param aTarget
         * @param aUseCopyCommand if ETrue does copy, else does move.
         * @return none.
         */
        void DoCopyOrMoveFileL( const TDesC& aSource, const TDesC& aTarget,
            const TBool aUseCopyCommand );
        
        /**
         * Check if folder can be showed
         * @param aFullPath The path of the object
         * @return ETrue if folder exists and can be showed.
         */
        TBool IsFolderVisible( const TDesC& aFullPath );
        
    private:
        CSConFsHandler( RFs& aFs );
        void ConstructL();
        /**
         * Updates the name of the data folder from a localized string
         * @param none
         * @return none
         */
        void GetDataPathNameL();
        
        /**
         * Check if folder is read only
         * @param aFullPath The path of the object
         * @return ETrue if folder is read only, or if folder does not exist
         */
        TBool IsFolderReadOnly( const TDesC& aFullPath ) const;
        /**
         * Check if folder can be deleted
         * @param aFullPath The path of the object
         * @return ETrue if folder can be deleted
         */
        TBool IsFolderDeletable( const TDesC& aFullPath ) const;
        /**
         * Get parent folder
         * @param aFullPath The folder full path with or without backlash ending
         * @param aParent The founded parent folder with backlash
         * @return KErrNone if parent folder founded
         */
        TInt GetParentFolder( const TDesC& aFullPath, TDes& aParent);
        
        /**
         * Get folder (and subfolders) size
         * @param aFullPath The folder full path with backlash ending
         * @return folder total size
         */
        TInt64 GetFolderSizeL( const TDesC& aFullPath );
        
        /**
         * Replaces all special characters to xml compliant ones
         * @param aDes characters to be checked and replaced
         */
        void ReplaceSpecialChars( TDes8& aDes );
        
        /**
         * Check if folder is exluded and cannot be shown to user
         * @param aFullPath The full path with backlash ending
         * @return ETrue if folder is exluded
         */
        TBool IsExludedPath( const TDesC& aFullPath ) const;
        
		/**
		 * Prints folder information to obex-folder-list
		 */
        void PrintFolderL( CBufFlat* aBuffer, TInt& aOffset, const TDesC& aParentPath,
                            const TDesC8& aMemType, const TEntry& aFolderEntry );
        /**
		 * Prints file information to obex-folder-list
		 */
        void PrintFileL( CBufFlat* aBuffer, TInt& aOffset, const TEntry& aFileEntry );
        
    private:
        RFs&                 iFs; // not own
        CDirectoryLocalizer* iLocalizer;
        TBuf8<KMaxFileName> iDataPathName;  // Localized data path
    };

#endif

// End of file

