/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CMmsAdapterMsvApi header
*
*/


#ifndef MMSADAPTERMSVAPI_H
#define MMSADAPTERMSVAPI_H

//  INCLUDES
#include <msvstd.h>         // TMsvId
 
// CONSTANTS
const TInt KMsvMyFoldersEntryIdValue = 0x1008;
const TInt KMsvMyFoldersTemplatesFolderId = 0x1009;

// FORWARD DECLARATIONS
class CMsvSession;


// CLASS DECLARATION

/**
* Class CMmsAdapterMsvApi
* Internal class that provides access to SMS message store.
*/
class CMmsAdapterMsvApi : public CBase 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param aMsvSession   IN: Access to message server
        *
        */
        static CMmsAdapterMsvApi* NewL(CMsvSession& aMsvSession);
                
        /**
        * Destructor.
        */
        virtual ~CMmsAdapterMsvApi();
        
    private: // Private constructor functions
        
        /**
        * C++ default constructor.
        * C++ default constructor can NOT contain any code, that
        * might leave.
        *
        * @param aMsvSession   IN: Access to message server
        *        
        */
        CMmsAdapterMsvApi(CMsvSession& aMsvSession);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();        

    public: // New functions
	    
         
        /**
        * Delete user folder under My Folders
        * 
        * @param aUid   IN: the index to the folder, which will be deleted
        *
        * @return   KErrNone in success. KErrInUse if folder was not deleted because it was not empty.                             
        */         
        TInt DeleteUserFolderL( TMsvId aUid );
	
       /**
        * Valid folder checks if the folder is valid.
        * The caller of the function can set, if Outbox folder is valid or not.
        * 
        * @param aFolder   folder
        * @param aOutboxValid if the MM is outbox is valid folder or not
        * 
        * @return ETrue when the folder is valid.
        * @return EFalse when the folder is not valid.
        */
        TBool ValidFolderL( TMsvId aFolder , TBool aOutboxValid = ETrue );		
		
       /**
        * Creates new user defined folder
        * 
        * @param aFolder    OUT: index of the new folder
        * @param aName      IN: name of the new folder
        *
        * @return     KErrNone or error
        */				
		TInt AddUserFolderL( TMsvId& aFolder, const TDesC& aName );
		
       /**
        * Updates name of user defined folder
        * 
        * @param aFolder    IN: index of the folder
        * @param aName      IN: new name of the folder
        *
        * @return     KErrNone or error
        */					
		TInt UpdateUserFolderL( TMsvId aFolder, const TDesC& aName );
		
        /**
        * Searches user folder with given UID
        * 
        * @param aFolder     IN: Folder UID
        * @param aName       OUT: name of found user folder
        * @param aDate       OUT: time stamp of found user folder 
        *
        * @return	Return value indicates whether folder was found or not
        */	        
        TBool FindUserFolderL(TMsvId aFolder, TPtrC& aName, TTime& aDate);	
        
        /**
        * Searches user folder with given UID
        * 
        * @param aFolder     IN: Folder UID
        *
        * @return	Return value indicates whether folder was found or not
        */	        
        TBool FindUserFolderL(TMsvId aFolder);
        
    private:	// Internal functions

        /**
        * Check whether folder with given name exists
        * 
        * @param aName     IN: Folder name
        * @parem aFolder   OUT: Folder UID if matching folder was found. 
        *
        * @return	Return value indicates whether folder was found or not 
        */	       
        TBool FindUserFolderL(const TDesC& aName, TMsvId& aFolder);		
    
    private:    // Data
        
        CMsvSession& 			iSession;
    			
    };

#endif      //SMSADAPTERMSVAPI_H
