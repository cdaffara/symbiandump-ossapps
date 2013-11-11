/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



#ifndef __CREATORPHONEBOOK_H__
#define __CREATORPHONEBOOK_H__

#include "creator_phonebookbase.h"
#include "creator_randomdatafield.h"
#include "creator_phonebookwrapper.h"



class CCreatorEngine;
class CCreatorModuleBaseParameters;

class CContactDatabase;	
class CPhonebookParameters;	
class CCreatorPhonebookWrapper;

class CCreatorPhonebook : public CCreatorPhonebookBase 
    {
public: 
    static CCreatorPhonebook* NewL(CCreatorEngine* aEngine);
    static CCreatorPhonebook* NewLC(CCreatorEngine* aEngine);
    ~CCreatorPhonebook();

private:
    CCreatorPhonebook();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase

public:  
    TInt CreateContactEntryL(CCreatorModuleBaseParameters *aParameters);    
    TInt CreateGroupEntryL(CCreatorModuleBaseParameters *aParameters);
    TInt CreateSubscribedContactEntryL(CCreatorModuleBaseParameters *aParameters); 

    
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();
    void DeleteAllGroupsL();
    void DeleteAllGroupsCreatedByCreatorL();
    
    void TestPrintOut(CPhonebookParameters* aParam);
    inline CCreatorPhonebookWrapper* GetPhonebookWrapper(){ return iPhonebookWrapper; };
private:
    
    void InitializeContactParamsL();
    TBool IsContactGroupL( TUint32& aLink );
    void StoreLinksForDeleteL( RArray<TUint32>& aLinks, TUid aStoreUid );
    void DeleteContactsL( RArray<TUint32>& aContactsToDelete, TUid aStoreUid );
    void DeleteItemsCreatedWithCreatorL( TUid aStoreUid );
    void DoDeleteItemsCreatedWithCreatorL( TUid aStoreUid, CDictionaryFileStore* aStore );
    TBool HasOtherThanGroupsL();
    

private:
	CCreatorPhonebookWrapper* iPhonebookWrapper;
    TInt iOpCounter;
    
    CPhonebookParameters* iParameters;
    
    static TInt iUrlFields[];
    static TInt iEmailFields[];
    
    RArray<TUint32> iContactLinkArray;
    RArray<TUint32>  iContactsToDelete;	
    RArray<TUint32>  iContactGroupsToDelete;	
    
    RArray<TUint32> iPreviousDeleteLinks;
    
private:
	//new variables
	/// Ref: the target of the copy
  
    
    /// Own: Contact database for this store
    CContactDatabase* iContactDb;    
    //Contacts found in contacts db.
    
    };

/**
 * phonebook parameters
 */
    

class CPhonebookParameters : public CCreatorModuleBaseParameters
    {
public:
	void ParseL(CCommandParser* parser, TParseParams /*aCase = 0*/);
	
	TInt CPhonebookParameters::ScriptLinkId() const;
	void CPhonebookParameters::SetScriptLinkId(TInt aLinkId);

	TCreatorContactFields iContactFields;

    HBufC*  iGroupName; 	
    TInt iContactsInGroup;
    TInt iNumberOfPhoneNumberFields;
    TInt iNumberOfURLFields;
    TInt iNumberOfEmailAddressFields;
    TInt iContactSetPtr;
    RArray<TLinkIdParam> iLinkIds; // For contactgroup. Stores the linked contact ids.
    
    
public:
    CPhonebookParameters();
    ~CPhonebookParameters();
    
private:
    TInt iLinkId; // For contact. Stores the contact id
    };


class CCreatorContactField : public CBase				//, public MCreatorRandomDataField
    {
public:    
    static CCreatorContactField* NewL();
    void AddFieldToParamL( CCreatorEngine* aEngine, CPhonebookParameters* aParam, TInt aType, TInt aRand = KErrNotFound );
    void AddFieldToParamL( CPhonebookParameters* aParam, TInt aType, TPtrC aContent );
    ~CCreatorContactField();
private:
    CCreatorContactField();
    void ConstructL();
    };

#endif // __CREATORPHONEBOOK_H__
