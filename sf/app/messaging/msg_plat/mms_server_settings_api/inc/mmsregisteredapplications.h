/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsregisteredapplications  declaration
*
*/



#ifndef __MMSREGISTEREDAPPLICATIONS_H
#define __MMSREGISTEREDAPPLICATIONS_H

//  INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <flogger.h>
#include <badesca.h>

// USERINCLUDE FILES
// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// CLASS DECLARATION
class CRepository;
/**
* CMmsRegisteredApplications takes care of registered applications in cen rep. 
* it keeps count on which application ids are registered.
* @lib mmsserversettings.lib
* @since 3.0
*/
class CMmsRegisteredApplications : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsRegisteredApplications* NewL();

        /**
        * Destructor
        */
        virtual ~CMmsRegisteredApplications();


    public: 
    
    public: // New functions

        /**
        * Loads registered applications array from Central Repository
        * In addition number of registered applications is loaded from cen rep.
        */
        IMPORT_C void LoadRegisteredApplicationsL();
        
        /**
        * Get Number of Registered Applications;
        * It is assumed that LoadRegisteredApplicationsL is called before this.
        * @return number of registered applications 
        */
        IMPORT_C TInt NumberOfRegisteredApplicationsL() const;
        
        /**
        * RegisteredL 
        *  
        * checks locally if the application id is registered or not
        * It is assumed that LoadRegisteredApplicationsL is called before this.
        * @since 3.0
        * 
        * @param aApplicationId application id
        * @return ETrue, if the application id is registered
        *         EFalse, if the application id is not registered.  
        */
        IMPORT_C TBool RegisteredL( const TDesC& aApplicationId );     
   
        /**
        * Register application.
        * the function adds an application id to the cen rep array.
        * If the application id is already registered, it is not added again.
        * It is assumed that LoadRegisteredApplicationsL is called before this.
        * @since 3.0
        * 
        * @param aApplicationId application id           
        * @return  KErrNone if successful.   
        */
        IMPORT_C TInt RegisterL( const TDesC& aApplicationId );
        
        /**
        * Unregister application
        * The function deletes the application id from the cenrep array.
        * It is assumed that LoadRegisteredApplicationsL is called before this.
        *  
        * @since 3.0
        * 
        * @param aApplicationId     application id           
        */
        IMPORT_C TInt UnregisterL( const TDesC& aApplicationId );
        
        /**
        * Return a list of registered applications
        *
        * It is assumed that LoadRegisteredApplicationsL is called before this.
        *
        * @since 3.1
        *
        * @return Descriptor array that contains ids of registered applications
        */
        IMPORT_C const CDesCArray& RegisteredApplicationList() const;
        
        /**
        * Register application.
        * the function adds an application id to the cen rep array.
        * If the application id is already registered, it is not added again.
        * It is assumed that LoadRegisteredApplicationsL is called before this.
        *
        * @since 3.2
        * 
        * @param aApplicationId application id           
        * @param aUserFriendlyName user friendly name for the application
        * @return  KErrNone if successful.   
        */
        IMPORT_C TInt RegisterL( const TDesC& aApplicationId,
            const TDesC& aUserFriendlyName );
        
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
        
    private:

        /**
        * C++ default constructor.
        */
        CMmsRegisteredApplications();

        /**
        * By default EPOC constructor is private.
        * @return void
        */
        void ConstructL();
        
        // New functions
        
        /**
        * gets the index of the registered application
        * If application is not registered, index will be -1
        * @param aApplicationId id to check
        * @param aIndex index of the id if registered
        * @return ETrue if registered, EFalse if not registered
        */
        TBool Registered( const TDesC& aApplicationId, TInt& aIndex );
        
        /**
        * Saves number of registered applicationsto cen rep
        */
        TInt SaveCount();

    public:     // Data
    protected:  // Data
    private:    // Data
    
  
    CDesCArrayFlat* iAppIdArray; // Array of registered applications
    CRepository* iRepository; // repository
    HBufC* iUserFriendlyName; // Last user friendly name

    };

#endif // __MMSREGISTEREDAPPLICATIONS_H   
            
// End of File
