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



#ifndef __CREATORPHONEBOOKWRAPPER_H__
#define __CREATORPHONEBOOKWRAPPER_H__

#include "creator_phonebookbase.h"
#include "creator_randomdatafield.h"

#include <map>

#include "creator_phonebookapi.h"

class CCreatorEngine;
class CCreatorModuleBaseParameters;

class CContactDatabase;	
//class CPhonebookWrapperParameters;	
class CPhonebookParameters;

typedef typename std::map<const TInt, HBufC*> TCreatorContactFields;
typedef typename std::pair<const TInt, HBufC*> TCreatorContactField;

class CCreatorPhonebookWrapper : public CBase 
    {
public:
	enum ContactDetailType{
	ELastName,					//0
	ELastNameReading,			//1
	EFirstName,
	EFirstNameReading,
	EPrefix,
	ESuffix,					//5
	ESecondName,
	ELandPhoneHome,
	EMobilePhoneHome,
	EVideoNumberHome,
	EFaxNumberHome,				//10
	EVoipHome,
	EEmailHome,
	EUrlHome,
	EAddrLabelHome,
	EAddrPoHome,				//15
	EAddrExtHome,
	EAddrStreetHome,
	EAddrLocalHome,
	EAddrRegionHome,
	EAddrPostCodeHome,			//20
	EAddrCountryHome,
	EJobTitle,
	ECompanyName,
	ELandPhoneWork,
	EMobilePhoneWork,			//25
	EVideoNumberWork,
	EFaxNumberWork,
	EVoipWork,
	EEmailWork,
	EUrlWork,					//30
	EAddrLabelWork,
	EAddrPoWork,
	EAddrExtWork,
	EAddrStreetWork,
	EAddrLocalWork,				//35
	EAddrRegionWork,
	EAddrPostCodeWork,
	EAddrCountryWork,
	ELandPhoneGen,
	EMobilePhoneGen,			//40
	EVideoNumberGen,
	EFaxNumberGen,
	EVoipGen,
	EPoc,
	ESwis,						//45
	ESip,
	EEmailGen,
	EUrlGen,
	EAddrLabelGen,
	EAddrPoGen,					//50
	EAddrExtGen,
	EAddrStreetGen,
	EAddrLocalGen,
	EAddrRegionGen,
	EAddrPostCodeGen,			//55
	EAddrCountryGen,
	EPagerNumber,
	EDtmfString,
	EWVAddress,
	EDate,						//60
	ENote,
	EThumbnailPic,
	ERingTone,
	ECallerObjImg,
	ECallerObjText,				//65
	EMiddleName,
	EDepartment,
	EAsstName,
	ESpouse,
	EChildren,					//70
	EAsstPhone,
	ECarPhone,
	EAnniversary,
	ESyncClass,
	ELocPrivacy,				//75
	EGenLabel,
	ETopContact,
	EIMPP,
	};

	
public: 
    static CCreatorPhonebookWrapper* NewL();
    static CCreatorPhonebookWrapper* NewLC();
    ~CCreatorPhonebookWrapper();

private:
    CCreatorPhonebookWrapper();
    void ConstructL();

public:  
    TUint32 CreateContactEntryL(const TCreatorContactFields& Map);    
    TUint32 CreateGroupEntryL( HBufC* aGroupName );
    TInt CreateSubscribedContactEntryL(); 

    /**
     * NumberOfContacts() will return all contacts from Contact API
     */
    TInt NumberOfContacts();
    
    TInt AddContactToGroup( TUint32 aGroupId, TUint32 aContact );
    TInt AddToGroup( TUint32 aGroupId, TInt aAmount );
    
    
    void DeleteAllL();
    void DeleteAllGroupsL();
    void DeleteContactsL( RArray<TUint32>& aContactsToDelete, TUid aStoreUid );
    
    HBufC* GetPhoneNumberL( TUint32 aContactId );
    TBool GetContactDetailsL( TUint32 aContactId, TDes& aName, TDes& aPhoneNumber, TDes& aEmail );
    
private:
    QList<QContactDetail> CreateContactDetailsFromParameters( const TCreatorContactFields& Map );
    QContactDetail CreateContactDetail( QList<QContactDetail>& aContactDetailList,QString aDetail, QString aFieldContext, QString aFieldString, QString aData );
    void AddFieldToList( QList<QContactDetail>& aDetailList, QContactDetail aDetail);
    
    TBool IsContactGroupL( TUint32& aLink );
    
    void DeleteItemsCreatedWithCreatorL( TUid aStoreUid );
    void DoDeleteItemsCreatedWithCreatorL( TUid aStoreUid, CDictionaryFileStore* aStore );
    TBool HasOtherThanGroupsL();
    

private:
	
    TInt iOpCounter;
    
    static QString iPhoneNumberFields[];
    static TInt iUrlFields[];
    static TInt iEmailFields[];
    
    RArray<TUint32> iContactLinkArray;
    RArray<TUint32>  iContactsToDelete;	
    RArray<TUint32>  iContactGroupsToDelete;	
    
    RArray<TUint32> iPreviousDeleteLinks;
    //RPointerArray<MVPbkContactLinkArray> iPreviousDeleteLinks;  //modify
    
private:
	//new variables
	/// Ref: the target of the copy
	QContact* iStore;       
    
    /// Own: Contact database for this store
    CContactDatabase* iContactDb;    
    
    //Contacts found in contacts db.
    QList<QContactId>* iContactResults;
    // Contact groups that are found in the store. These are used in filtering
    // the groups from the find results.
    QList<QContactId>* iContactGroupsInStore;
    
    CCreatorPhonebookAPI* iPhonebookAPI;
    };

#endif // __CREATORPHONEBOOKWRAPPER_H__
