/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contacts DataStore Extension API
*
*/

#ifndef NSMLCONTACTSDATASTOREEXTENSION_H_
#define NSMLCONTACTSDATASTOREEXTENSION_H_

class CNsmlContactsDataStoreExtensionPrivate;
class CNsmlContactItem;
class RDesReadStream;

class CNsmlContactsDataStoreExtension: public CBase
{
public:
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CNsmlContactsDataStoreExtension* NewL( const TDesC& aManager );

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CNsmlContactsDataStoreExtension();
    
protected:
    /**
    * C++ constructor.
    */
    IMPORT_C CNsmlContactsDataStoreExtension();
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CNsmlContactsDataStoreExtension* NewLC( const TDesC& aManager );
    /**
    * 2nd phase constructor.
    */
    IMPORT_C void ConstructL( const TDesC& aManager );
    
public: // Methods
    /**
    * Fetch the Contact Item from the DB 
    * @param aUid UID of the desired item to be fetched
    * @param aContactbufbase on return will have the fetched Contact details
    */
    IMPORT_C TInt ExportContactsL( const TUid& aUid, CBufBase& aContactbufbase );
    
    /**
    * Store the Contact Item to the DB
    * @param aContactbufbase Contact item to be saved to the DB
    * @return A pointer to list of newly saved Contact UIDs'
    */
    IMPORT_C CArrayFixFlat<TUid>* ImportContactsL( const TDesC8& aContactbufbase );
    
    /**
    * Fetch Contact details
    * @param aUid UID of the desired Contact item
    * @param aContactitem on return will have pointer class containing contact details
    */
    IMPORT_C TInt ReadContactL( const TUid& aUid, CNsmlContactItem* aContactitem );
    
    /**
    * Delete a Contact
    * @param aUid UID of the desired Contact item
    * @return status of the operation
    */
    IMPORT_C TBool DeleteContactL( const TUid& aUid );
    
    /**
    * Delete List of Contacts
    * @param aArrDelete List of Contact UIDs' to be deleted
    * @return status of the operation
    */
    IMPORT_C TBool DeleteContactsL( CArrayFixFlat<TUid>* aArrDelete );
    
    /**
    * Delete All Contacts
    * @return status of the operation
    */
    IMPORT_C TBool DeleteAllContactsL();
    
    /**
    * List Supported Stores
    * @param aCntstores on return will have list of stores
    */
    IMPORT_C void ListStoresL( CDesCArray* aCntstores );
    
    /**
    * Unique ID for a Store
    * @return unique id represented as integer  
    */
    IMPORT_C TInt64 MachineIdL();
    
    /**
    * Fetch all available Contacts from the DB
    * @param aContactuidlist on return will have list of all Contact UIDs'
    */
    IMPORT_C void ListContactsL( CArrayFixFlat<TUid>* aContactuidlist );
	
private: // Data
	CNsmlContactsDataStoreExtensionPrivate* d_ptr;
};

class CNsmlContactItem: public CBase
{
public:
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CNsmlContactItem* NewL();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CNsmlContactItem();
    
protected:
    /**
    * C++ constructor.
    */
    IMPORT_C CNsmlContactItem();
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CNsmlContactItem* NewLC();
 
public: // Data
    TTime iLastModified;
    HBufC8* iGuid;
    TUid iUid;
};

#endif /* NSMLCONTACTSDATASTOREEXTENSION_H_*/
