/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConFTP header file
*
*/


#ifndef _SCONFTP_H_
#define _SCONFTP_H_

// INCLUDES

#include <e32base.h>
#include <obexobjects.h>

#include "sconconstants.h"

class CSConInboxHandler;
class CSConFsHandler;

//============================================================
// Class CSConFTP declaration
//============================================================
class CSConFTP : public CBase
    {
    public:
        // construct / destruct
        static CSConFTP* NewL();        
        ~CSConFTP();

        // methods
         /**
         * Gets object that contains folder listing from inbox or file system
         * @param aObject CObexBufObject object
         * @return KErrNone if no errors, else system wide error codes.
         */
         virtual TInt GetFolderObjectL( CObexBufObject*& aObject );

         /**
         * Gets object that contains a file from file system
         * @param aFileObject CObexFileObject object
         * @return KErrNone if no errors, else system wide error codes.
         */
         virtual TInt GetFileObjectL( CObexFileObject*& aFileObject );

         /**
         * Initializes the receiving from the client.
         * @param aObject CObexBufObject object
         * @param aObject CBufFlat object
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt PutFileObjectInitL( CObexBufObject*& aObject, 
                                            CBufFlat*& aBuffer );

         /**
         * Stores the relayed file object to inbox or file system.
         * @param aObject CObexBufObject object
         * @return KErrNone if no error, else system wide error codes.        
         */
         virtual TInt PutFileObjectFinalizeL( CObexBufObject*& aObject );

         /**
         * Changes the current path. The path can point to inbox or file system
         * @param aPathName The name of the new path
         * @param aFlags Flag value of OBEX Setpath operation
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt SetPathL( const TPtrC aPathName, const TUint8 aFlags );

         /**
         * Creates a new folder to file system
         * @param aFolderName The name of the folder to be created
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt CreateFolderL( const TPtrC aFolderName );

         /**
         * Deletes file/folder from inbox or file system
         * @param aObjectName The name of the object (file or folder) to be created
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt DeleteObjectL( const TPtrC aObjectName );

         /**
         * Gets the current path
         * @param aPath The name of the current path
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt GetPath( TDes& aPath );
         
         /**
         * Abort file transfer
         * @param aObject The OBEX stack object which will reseted
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt AbortFileTransfer( CObexBufObject*& aObject );

         /**
         * Set used transfer profile
         * @param aProfile The profile id
         * @return none
         */
         virtual void SetProfile( TSConProfile aProfile );

         /**
         * Set backup status
         * @param aValue Is backup started or not
         * @return none
         */
         virtual void SetBackupStarted( TBool aValue );
         
         /**
         * Set the used media information
         * @param aMedia The media which is currently used
         * @return none
         */
         virtual void SetUsedMedia( TSConUsedMedia aMedia );

         /**
         * Check is current drive space below critical level
         * @param aFilesize, additional free space needed
         * @return ETrue if below critical                                                  
         */
         virtual TBool IsCurrentDiskSpaceBelowCritical( TUint32 aFilesize = 0 );
         
         /**
         * Read received ConML protocol packet
         * @param aBuffer The buffer for the data
         * @return KErrNone if no error, else system wide error codes.                                                                        
         */
         virtual TInt ReadWBXMLDataL( CBufFlat*& aBuffer );
         
         /**
         * Delete OBEX stack's temp file
         * @param none
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual void DeleteTempFile();
         
         /**
         * Move or rename file/folder
         * @param aSource
         * @param aTarget
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt MoveFile( const TDesC& aSource, const TDesC& aTarget );
         
         /**
         * Copy file/folder
         * @param aSource
         * @param aTarget
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt CopyFile( const TDesC& aSource, const TDesC& aTarget );
         
         /**
         * Set read-only attribute to file or folder
         * @param aTarget file or folder fullname
         * @param aReadOnly set read-only permission to ETrue or EFalse
         * @return KErrNone if no error, else system wide error codes.
         */
         virtual TInt SetReadOnly( const TDesC& aTarget, const TBool aReadOnly );
         
        /**
         * Set hidden attribute to file or folder
         * @param aTarget file or folder fullname
         * @param aHidden set read-only permission to ETrue or EFalse
         * @return KErrNone if no error, else system wide error codes.
         */
        virtual TInt SetHidden( const TDesC& aTarget, const TBool aHidden );
         
    private:
        // methods
        /**
         * Get absolute path from given relative foldername
         * @param aFolderName relative or absolute filename
         * @param aFullPath return absolete path of file/folder
         * @return KErrNone if no error, else system wide error codes.
        */
        virtual TInt GetAbsolutePath( const TDesC& aFolderName, TDes& aFullPath );
        
        /**
         * Checks if user can see/access to the drive
         * @param aDriveNumber drive to be tested
         * @return ETrue if user can see drive.
        */
        TBool IsDriveVisible( const TInt aDriveNumber );
        
        /**
         * Checks if user access to the current volume
         * @param none
         * @return ETrue if volume is ok.
        */
        TBool IsCurrentVolumeOK();
        
        /**
         * Go to (sub)directory
         * @param aPath (sub)directory
         * @param aFlags used to check if new folder(s) should be created
         * @return KErrNone if no error, else system wide error codes.
        */
        TInt SetPathForwardL( const TPtrC aPath, const TUint8 aFlags );
        
        /**
         * Go to parent directory
         * @param aPath subdirectory under parent (additional)
         * @return KErrNone if no error, else system wide error codes.
        */
        TInt SetPathBackwardL( const TPtrC aPath );
        
        /**
         * Get volume name (Memory card name)
         * @param aDriveNumber drive where to read
         * @param aVolumeName if name found return volume name, else localised string
         * @return KErrNone if no error, else system wide error codes.
        */
        TInt GetVolumeNameL(const TInt aDriveNumber, TDes8& aVolumeName);
        
        /**
         * Updates drive information (iCurrentDrive and iCurrentDriveTypeNumber)
         * @param none
         * @return none, leaves if error occurs.
        */
        void UpdateDriveTypeInfoL();
        
        void CreateObexBufObjectL( CObexBufObject*& aObject, CBufFlat*& aBuffer );
        
        CSConFTP();
        virtual void ConstructL();
    
    private:
        CSConInboxHandler*                  iSConInboxHandler;  // Object saving-fetching
        CBufFlat*                           iBuffer;            // Temporary buffer
        CSConFsHandler*                     iSConFsHandler;     // Object saving-fetching
        TBuf<KObexObjectDescriptionSize>    iPathName;          // Path
        TFileName                           iTempFileName;      // Temp file name
        RFs                                 iFs;
        TSConProfile                        iProfile;           // Used profile
        TBool                               iBackupStarted;
        TSConUsedMedia                      iMedia;
        TInt                                iCurrentDrive;
        TInt                                iCurrentDriveTypeNumber; // DEV=1, DEV2=2, MMC=1, MMC2=2.
        TInt                                iCriticalDiskLevel;
    };

    IMPORT_C CSConFTP* CreateCSConFTPL();
    typedef CSConFTP* (*TSConCreateCSConFTPFunc) ();   


#endif // SCONFTP

// End of file
