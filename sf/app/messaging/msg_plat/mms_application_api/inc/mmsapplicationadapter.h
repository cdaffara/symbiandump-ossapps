/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   mms application adapter
*
*/




#ifndef MMSAPPLICATIONADAPTER_H
#define MMSAPPLICATIONADAPTER_H

//  INCLUDES
#include <msvstd.h>         // TMsvId
#include <msvapi.h>         // MMsvSessionObserver
#include <badesca.h>        // descriptor array

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMmsSettings;
class CMmsRegisteredApplications;
class CMmsHeaders;

// CLASS DECLARATION

/**
*  
*
*  @lib mmsappadapter.lib
*  @since 3.0
*/

class CMmsApplicationAdapter : public CBase, public MMsvSessionObserver 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsApplicationAdapter* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CMmsApplicationAdapter();

    public: // New functions
    
        /**
        * Register application.
        *  
        * @since 3.0
        * 
        * @param aApplicationId application id           
        * @param aFolderId folder id for this application id. 
        * @return  KErrNone if successful.    
        */
        IMPORT_C TInt RegisterL( const TDesC& aApplicationId, TMsvId& aFolderId  );
        
        /**
        * Unregister application
        * The function destroys the corresponding folder and its content.
        *  
        * @since 3.0
        * 
        * @param aApplicationId     application id           

        * @return  KErrNone if successful
        */
        IMPORT_C TInt UnregisterL( const TDesC& aApplicationId );
        
        /**
        * RegisteredL 
        *  
        * checks if the application id is registered or not
        * @since 3.0
        * 
        * @param aApplicationId application id
        * @return ETrue, if the application id is registered
        *		  EFalse, if the application id is not registered.  
        */
        IMPORT_C TBool RegisteredL( const TDesC& aApplicationId );
        
        /**
        * Folder Entry ID 
        *  
        * @since 3.0
        * 
        * @param aApplicationId application id
        * @return the index of the folder entry 
        */
        IMPORT_C TMsvId FolderIdL( const TDesC& aApplicationId );
    
        /**
        * Application ID 
        *  
        * @since 3.0
        * 
        * @param aMmId     Index to the multimedia message
        
        * @return   application id
        */
        IMPORT_C TPtrC16 ApplicationIdL( TMsvId aMmId );
            
        /**
        * Set application ID 
        *  
        * @since 3.0
        * 
        * @param aMmId     Index to the multimedia message
        * @param aApplicationId application id
        * @return  
        */
        IMPORT_C void SetApplicationIdL( 
            TMsvId aMmId,
            const TDesC& aApplicationId );
            
        /**
        * Reply to Application ID 
        *  
        * @since 3.0
        * 
        * @param aMmId     Index to the multimedia message
        
        * @return   reply to application id
        */
        IMPORT_C TPtrC16 ReplyToApplicationIdL( TMsvId aMmId );
            
        /**
        * Set reply to application ID 
        *  
        * @since 3.0
        * 
        * @param aMmId     Index to the multimedia message
        * @param aReplyToApplicationId reply to application id
        * @return  
        */
        IMPORT_C void SetReplyToApplicationIdL( 
            TMsvId aMmId,
            const TDesC& aReplyToApplicationId ); 
                     
        /**
        * Delete the specified MM.
        *  
        * @since 3.0
        * 
        * @param aMmId     Index to the MM, which will be deleted.
        *
        * @return   KErrNone if successful
        */
        IMPORT_C TInt DeleteL( TMsvId aMmId );
        
        /**
        * MmscUrlL
        *
        * @since 3.0
        *
        * @param HBufC*& aServerUri
        *
        * Usage:
        * aServerUri should not be allocated by the caller of this method.
        * The buffer is pushed onto CleanupStack temporarily. 
        * The caller is responsible for destroying the buffer 
        * when the buffer is no longer needed.
        * 
        * @return   KErrNone if successful
        * KErrNotFound if not found
        * KErrGeneralError, something has failed.
        *  
        * Note that if the uri is not defined, KErrNone is returned.
        */
        IMPORT_C TInt MmscUrlL( HBufC*& aServerUri ); 
            
        /**
        * Return a list of registered applications
        *
        * @since 3.1
        *
        * @return Descriptor array that contains ids of registered applications
        */
        IMPORT_C const CDesCArray& RegisteredApplicationList() const;
        
        /**
        * Register application.
        *  
        * @since 3.2
        * 
        * @param aApplicationId application id           
        * @param aUserFriendlyName user friendly name for the application
        * @param aFolderId folder id for this application id (return value). 
        * @return KErrNone if successful.    
        */
        IMPORT_C TInt RegisterWithNameL(
            const TDesC& aApplicationId,
            const TDesC& aUserFriendlyName,
            TMsvId& aFolderId );
            
        /**
        * Get User Friendly Name for an application
        *
        * @since 3.2
        * @param aApplicationId application id
        * @return User Friendly Name for the application.
        *     If a user friendly name cannot be found, returns empty string.
        *     The pointer is valid only as long as CMmsRegisteredApplications
        *     is alive and as long as the function is not called again.
        *     (It is a pointer to member variable)
        *     The caller should allocate memory and copy the string if it is needed
        *     for longer period of time or if names for several applications are 
        *     needed.
        */
        IMPORT_C TPtrC UserFriendlyName( const TDesC& aApplicationId );
   
                           

    public: // Functions from base classes

        
    protected:  // New functions
        
        
    protected:  // Functions from base classes

        /**
        * From MMsvSessionObserver.
        * @since 3.0
        * 
        * @param aEvent    the caller of the method sets the value.
        * @param aArg1     depends on the aEvent value
        * @param aArg2     depends on the aEvent value
        * @param aArg3     depends on the aEvent value
        * 
        */

        virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
        
        
    private: // New functions
    
        /**
        * C++ default constructor.
        * C++ default constructor can NOT contain any code, that
        * might leave.
        * @since 3.0
        */
        CMmsApplicationAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 3.0
        */
        void ConstructL();
        
               
    private:

        

        /**
        * Functions from CMmsBaseOperation 
        */

        

        
    public:     // Data
    protected:  // Data
    private:    // Data
    
    CMsvSession* iSession; // channel for client side thread and message server
    CMsvEntry* iClientEntry;
    
    CMmsHeaders* iMmsHeaders;
    CMmsSettings* iMmsSettings;
    CMmsRegisteredApplications* iRegAppIds;

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSAPPLICATIONADAPTER
            
// End of File
