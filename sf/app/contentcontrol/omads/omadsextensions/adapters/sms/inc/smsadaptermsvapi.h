/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSmsAdapterMsvApi header
*
*/



#ifndef SMSADAPTERMSVAPI_H
#define SMSADAPTERMSVAPI_H

//  INCLUDES
#include <e32base.h>        // CActive
#include <msvstd.h>         // TMsvId
#include <msvapi.h>         // CMsvOperation
#include <mtclreg.h>		//CClientMtmRegistry
#include <msvids.h>
#include <mtclbase.h>
#include <msvuids.h>
#include <smut.h>
 
// FORWARD DECLARATIONS
class CSmsClientMtm;
class CVMessageParser;
class CContactDatabase;

// CONSTANTS
const TInt KMsvMyFoldersEntryIdValue = 0x1008;
const TInt KMsvMyFoldersTemplatesFolderId = 0x1009;

// CLASS DECLARATION

/**
* Class CSmsAdapterMsvApi
* Internal class that provides access to SMS message store.
*/
class CSmsAdapterMsvApi : public CBase, public MMsvSessionObserver 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
         static CSmsAdapterMsvApi* NewL();
                
        /**
        * Destructor.
        */
        virtual ~CSmsAdapterMsvApi();
        
    private: // Private constructor functions
        
        /**
        * C++ default constructor.
        * C++ default constructor can NOT contain any code, that
        * might leave.
        */
        CSmsAdapterMsvApi();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();        

    public: // New functions
	    
        /**
        * Add a SMS message. The given SMS message is stored to the target folder. 
        * 
        * @param aSm       IN: Contains SMS message in decoded format
        * @param aFolder   IN: The folder in which the SM will be stored 
        * @param aSmId     OUT: The index created for the added SM.
        */
		 void AddSML( 
			CVMessageParser& aSm,
            TMsvId aFolder,                                                                                                                                                                                                          
            TMsvId& aSmId );

        /**
        * Replace existing SMS message
        * 
        * @param aSmId      IN: the index to the replaceable SM  
        * @param aSm        IN: Contains data of the new SM        
        */
         void ReplaceSML (
            TMsvId aSmId, 
			CVMessageParser& aSm ); 

        /**
        * Delete a SMS message. 
        * 
        * @param aSmId   IN: the index to the SM, which will be deleted                             
        */
        void DeleteSML( TMsvId aSmId );
         
        /**
        * Delete a SMS message. 
        * 
        * @param aUid   IN: the index to the SM, which will be deleted
        *
        * @return   KErrNone in success. KErrInUse if folder was not deleted because it was not empty.                             
        */         
        TInt CSmsAdapterMsvApi::DeleteUserFolderL( TMsvId aUid );

        /**
        * Retriveve SMS message from the message store.
        * 
        * @param aSmId      IN: Index to the SM, which will be retrieved    
        * @param aParent    OUT: Index of parent folder        
        * @param aSm        OUT: The SM data is stored into this object.
        * @param aUnread    OUT: Whether the SM is unread or not 
        */
         void RetrieveSML(
            TMsvId aSmId,
            TMsvId& aParent,
            CVMessageParser& aSM,                  
            TBool& aUnread );

        /**
        * Sends the specified SM.
        *  
        * @param aSmId     IN: Index to the SM, which will be sent.
        */
         void SendSML( TMsvId aSmId );

        /**
        * This function can be used when client needs direct access to CMsvSession.
        * @return	 ponter to opened message server session.
        */
		 CMsvSession* MsvSessionL();

       /**
        * Empties folder from SMS messages.
        *
        * @param    aFolderId   IN: TMsvId of the folder
        */
        void CleanFolderL( TMsvId aFolderId);
        
       /**
        * Empties user folders under My Folders from SMS messages
        */       
        void CSmsAdapterMsvApi::CleanUserFoldersL(); 
        
       /**
        * Empties folder from SMS messages and returns list of deleted messages
        * Caller is responsible of deleting returned list.
        *
        * @param    aFolderId   IN: TMsvId of the folder
        *
        * @return   List of identifiers of deleted messages
        */       
        CMsvEntrySelection* CleanFolderGetMsvIdsL(TMsvId aFolderId);

       /**
        * Checks whether disk space is below critical level.
        *
        * @param    aDataSize	Size of data to be added to disk.
        */
		TBool DiskSpaceBelowCriticalLevelL( TInt aDataSize );

        /**
        * Updates SM status in inbox
        * 
        * @param aSmId      IN: the index to the SM  
        * @param aSm        IN: Contains status of the new SM        
        */
		void UpdateSMStatusL( TMsvId aSmId, CVMessageParser& aSm );
	
        /**
        * Moves SM to another folder
        * 
        * @param aSmId      IN: the index to the SM  
        * @param aParentId  IN: Contains the target folder 
        */		
		void MoveSML( TMsvId aSmId, TMsvId aParentId );
		
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
        * @param aFound      OUT: name of found user folder
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
        
    protected:  // Functions from base classes
        
        /**
        * From MMsvSessionObserver. Message server event handler function.
        * 
        * @param aEvent    the caller of the method sets the value.
        * @param aArg1     depends on the aEvent value
        * @param aArg2     depends on the aEvent value
        * @param aArg3     depends on the aEvent value
        * 
        */
        virtual void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    private:	// Internal functions

        /**
        * Updates existing SMS message with given values.
		* Used internally by AddSML and ReplaceSML
        * 
        * @param aSmId      IN: the index to the SM  
        * @param aSm        IN: contains data of the SM        
		* @param aNewSm		IN: indicates whether this SM is a new one 
        */
		void DoUpdateSML( TMsvId aSmId, CVMessageParser& aSm, TBool aNewSm );
         
        /**
        * Check whether folder with given name exists
        * 
        * @param aName     IN: Folder name
        * @parem aFolder   OUT: Folder UID if matching folder was found. 
        *
        * @return	ETrue or EFalse 
        */	       
        TBool FindUserFolderL(const TDesC& aName, TMsvId& aFolder);

        /**
        * Test whether given character needs unicode coding when constructing
		* a SMS message. 
		* 7-bit coding is used if there are no unicoder characters in the message.
        * 
        * @param aValue     IN: Given character value   
        *         
		* @return	ETrue if unicode coding is needed, EFalse otherwise
        */
		TBool IsUnicode( const TUint16 aValue );

        /**
        * Convert given home time to universal time.
        * 
        * @param aTime     IN: Home time 
        *
        * @return	Universal time   
        */		
		TTime UniversalTimeFromHomeTime( TTime aTime );

        /**
        * Convert given universal time to home time.
        * 
        * @param aTime     IN: Universal time
        *
        * @return 	Home time   
        */				
		TTime HomeTimeFromUniversalTime( TTime aTime );
		
        /**
        * Find contact name from address book for given number
        * 
        * @param aTime     IN: Phone number
        * @param aName     OUT: Contact name, empty if suitable contact not found
        */			
		void FetchNameFromContactsL(const TDesC& aNumber, TDes& aName);
		
    
    private:    // Data
        
        CMsvSession* 			iSession;
        CClientMtmRegistry* 	iMtmReg;
		CSmsClientMtm* 			iMtm;

		RFs		iFs;
		TInt	iMessageDrive;
		
		CContactDatabase* 	iContactsDb;
		
    };

#endif      //SMSADAPTERMSVAPI_H
