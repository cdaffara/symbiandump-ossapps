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


#include "engine.h"
#include "enginewrapper.h"
#include "creator_contactsetcache.h"

#include "creator_phonebook.h"
#include "creator_traces.h"

#include <bautils.h> 

_LIT(KTempPath, "C:\\Data\\Creator\\");


typedef struct {
TInt iFieldCode;
TInt iRandomType;
} DetailFieldInfo;
static const TInt RND_TYPE_UNDEF = -99;
DetailFieldInfo CreatorPbkMiscTextFields[] = {        
        {CCreatorPhonebookWrapper::EFirstName, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::EFirstNameReading, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::ELastName, (TInt) CCreatorEngine::ESurname},
        {CCreatorPhonebookWrapper::ELastNameReading, (TInt) CCreatorEngine::ESurname},
        {CCreatorPhonebookWrapper::ECompanyName, (TInt) CCreatorEngine::ECompany},
        {CCreatorPhonebookWrapper::EJobTitle, (TInt) CCreatorEngine::EJobTitle},
        {CCreatorPhonebookWrapper::EPrefix, (TInt) CCreatorEngine::EPrefix},
        {CCreatorPhonebookWrapper::ESuffix, (TInt) CCreatorEngine::ESuffix},
        {CCreatorPhonebookWrapper::ESecondName, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::EAddrLabelGen, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrPoGen, (TInt) CCreatorEngine::EPobox},
        {CCreatorPhonebookWrapper::EAddrExtGen, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrStreetGen, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrLocalGen, (TInt) CCreatorEngine::ECity},
        {CCreatorPhonebookWrapper::EAddrRegionGen, (TInt) CCreatorEngine::EState},
        {CCreatorPhonebookWrapper::EAddrPostCodeGen, (TInt) CCreatorEngine::EPostcode},
        {CCreatorPhonebookWrapper::EAddrCountryGen, (TInt) CCreatorEngine::ECountry},
        {CCreatorPhonebookWrapper::EAddrLabelHome, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrPoHome, (TInt) CCreatorEngine::EPobox},
        {CCreatorPhonebookWrapper::EAddrExtHome, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrStreetHome, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrLocalHome, (TInt) CCreatorEngine::ECity},
        {CCreatorPhonebookWrapper::EAddrRegionHome, (TInt) CCreatorEngine::EState},
        {CCreatorPhonebookWrapper::EAddrPostCodeHome, (TInt) CCreatorEngine::EPostcode},
        {CCreatorPhonebookWrapper::EAddrCountryHome, (TInt) CCreatorEngine::ECountry},
        {CCreatorPhonebookWrapper::EAddrLabelWork, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrPoWork, (TInt) CCreatorEngine::EPobox},
        {CCreatorPhonebookWrapper::EAddrExtWork, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrStreetWork, (TInt) CCreatorEngine::EAddress},
        {CCreatorPhonebookWrapper::EAddrLocalWork, (TInt) CCreatorEngine::ECity},
        {CCreatorPhonebookWrapper::EAddrRegionWork, (TInt) CCreatorEngine::EState},
        {CCreatorPhonebookWrapper::EAddrPostCodeWork, (TInt) CCreatorEngine::EPostcode},
        {CCreatorPhonebookWrapper::EAddrCountryWork, (TInt) CCreatorEngine::ECountry},
        {CCreatorPhonebookWrapper::EPoc, (TInt) CCreatorEngine::EPhoneNumber},
        {CCreatorPhonebookWrapper::ESwis, (TInt) CCreatorEngine::EPhoneNumber},
        {CCreatorPhonebookWrapper::ESip, (TInt) CCreatorEngine::EPhoneNumber},
        {CCreatorPhonebookWrapper::EDtmfString, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::ENote,(TInt)  CCreatorEngine::EMemoText},
        {CCreatorPhonebookWrapper::EMiddleName, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::EDepartment, (TInt) CCreatorEngine::ECompany},
        {CCreatorPhonebookWrapper::EAsstName, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::ESpouse, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::EChildren, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::ESyncClass, RND_TYPE_UNDEF},
        {CCreatorPhonebookWrapper::ELocPrivacy, RND_TYPE_UNDEF},
        {CCreatorPhonebookWrapper::EGenLabel, (TInt) CCreatorEngine::EFirstName},
        {CCreatorPhonebookWrapper::EWVAddress, (TInt) CCreatorEngine::EPhoneNumber},
        {CCreatorPhonebookWrapper::ERingTone, RND_TYPE_UNDEF},
        {CCreatorPhonebookWrapper::EThumbnailPic, RND_TYPE_UNDEF},
        {CCreatorPhonebookWrapper::ECallerObjText, (TInt) CCreatorEngine::EFirstName}
        };
                  

TInt CreatorPbkBinaryFields[] = {
        CCreatorPhonebookWrapper::ECallerObjImg//,
        //R_VPBK_FIELD_TYPE_THUMBNAILPATH
        };

TInt CreatorPbkDateTimeFields[] = {
        CCreatorPhonebookWrapper::EAnniversary
        };

//----------------------------------------------------------------------------
TInt CreatorPbkPhoneNumberFields[] =
    {
    CCreatorPhonebookWrapper::ELandPhoneGen,
    CCreatorPhonebookWrapper::ELandPhoneHome,
    CCreatorPhonebookWrapper::ELandPhoneWork,                        
    CCreatorPhonebookWrapper::EMobilePhoneGen,
    CCreatorPhonebookWrapper::EMobilePhoneHome,
    CCreatorPhonebookWrapper::EMobilePhoneWork,
    CCreatorPhonebookWrapper::EFaxNumberGen,
    CCreatorPhonebookWrapper::EFaxNumberHome,
    CCreatorPhonebookWrapper::EFaxNumberWork,
    CCreatorPhonebookWrapper::EPagerNumber,           
    CCreatorPhonebookWrapper::EVideoNumberGen,
    CCreatorPhonebookWrapper::EVideoNumberHome,
    CCreatorPhonebookWrapper::EVideoNumberWork,
    CCreatorPhonebookWrapper::EVoipGen,
    CCreatorPhonebookWrapper::EVoipHome,
    CCreatorPhonebookWrapper::EVoipWork,
    CCreatorPhonebookWrapper::EAsstPhone,
    CCreatorPhonebookWrapper::ECarPhone
    };

TInt CreatorPbkUrlFields[] =
    {
    CCreatorPhonebookWrapper::EUrlGen,
    CCreatorPhonebookWrapper::EUrlHome,
    CCreatorPhonebookWrapper::EUrlWork
    };

TInt CreatorPbkEmailFields[] =
    {
    CCreatorPhonebookWrapper::EEmailGen,
    CCreatorPhonebookWrapper::EEmailHome,
    CCreatorPhonebookWrapper::EEmailWork
    };

CPhonebookParameters::CPhonebookParameters()
    {
    LOGSTRING("Creator: CPhonebookParameters::CVirtualPhonebookParameters");
    iGroupName = HBufC::New(KPhonebookFieldLength);  
    }

CPhonebookParameters::~CPhonebookParameters()
    {
    LOGSTRING("Creator: CPhonebookParameters::~CPhonebookParameters");

    delete iGroupName;   
    for( TCreatorContactFields::iterator it = iContactFields.begin(); it != iContactFields.end(); ++it)
        {
        HBufC* temp =  (*it).second;
        iContactFields.erase(it);
        delete temp;
        }
    iLinkIds.Reset();
    iLinkIds.Close();
    }

void CPhonebookParameters::ParseL(CCommandParser* /*parser*/, TParseParams /*aCase = 0*/)
	{
	}

TInt CPhonebookParameters::ScriptLinkId() const
    {
    return iLinkId;
    }

void CPhonebookParameters::SetScriptLinkId(TInt aLinkId)
    {
    iLinkId = aLinkId;
    }


//----------------------------------------------------------------------------

CCreatorPhonebook* CCreatorPhonebook::NewL(CCreatorEngine* aEngine)
    {
    CCreatorPhonebook* self = CCreatorPhonebook::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorPhonebook* CCreatorPhonebook::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorPhonebook* self = new (ELeave) CCreatorPhonebook();
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }


CCreatorPhonebook::CCreatorPhonebook() 
    {
    iAddAllFields = EFalse;
    }

void CCreatorPhonebook::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorPhonebook::ConstructL");
	
	iEngine = aEngine;
	
	iPhonebookWrapper = CCreatorPhonebookWrapper::NewL();

	SetDefaultParameters();
    }
	

CCreatorPhonebook::~CCreatorPhonebook()
    {
    LOGSTRING("Creator: CCreatorPhonebook::~CCreatorPhonebook");
 
    if ( iContactsToDelete.Count() )
    	{
        TRAP_IGNORE( StoreLinksForDeleteL( iContactsToDelete, KUidDictionaryUidContacts ) );
        }
    if ( iContactGroupsToDelete.Count() )
    	{
        TRAP_IGNORE( StoreLinksForDeleteL( iContactGroupsToDelete, KUidDictionaryUidContactGroups ) );
        }
    
	if( iPhonebookWrapper )
		{
		delete iPhonebookWrapper;
		}
    
    if (iParameters)
    	{
        delete iParameters;
    	}
  
    }

//----------------------------------------------------------------------------

TInt CCreatorPhonebook::CreateContactEntryL(CCreatorModuleBaseParameters *aParameters)
    {
	TInt err = KErrNone;
	delete iParameters;
	    iParameters = 0;
	CPhonebookParameters* parameters = 0;
	
	if( aParameters == 0 )
	    {
	    InitializeContactParamsL();
	    parameters = iParameters;
	    }
	else
	    {
	    parameters = (CPhonebookParameters*) aParameters;
	    }
	    
	int numberOfFields = parameters->iContactFields.size();
	// create a new contact item
	
	TUint32 contactId = iPhonebookWrapper->CreateContactEntryL( parameters->iContactFields ); //wrapper should return contact id to store
	iContactsToDelete.AppendL( contactId );
	

	// If this contact has a link id in script (i.e. belongs to a contact-set), we must cache the contact id:
	    if( parameters->ScriptLinkId() > 0 )
	        {                
	        RPointerArray<CCreatorContactSet>& contactsets = ContactLinkCache::Instance()->ContactSets();
	        TBool setFound(EFalse);
	        for(TInt i = 0; i < contactsets.Count(); ++i )
	            {
	            if( contactsets[i]->LinkId() == parameters->ScriptLinkId() )
	                {
	                if( contactId )
	                    {
	                    contactsets[i]->AppendL( contactId );
	                    iContactLinkArray.AppendL( contactId );
	                    }
	                setFound = ETrue;
	                break;
	                }
	            }
	        if( !setFound )
	            {
	            LOGSTRING2("Error: Contact set id %d not found.", parameters->ScriptLinkId());
	            }
	        }
	    
    return err;
    }


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

void CCreatorPhonebook::DeleteAllL()
    {
	iPhonebookWrapper->DeleteAllL();
    }

//----------------------------------------------------------------------------
void CCreatorPhonebook::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorPhonebook::DeleteAllCreatedByCreatorL");
    DeleteItemsCreatedWithCreatorL( KUidDictionaryUidContacts );
    }

//----------------------------------------------------------------------------
void CCreatorPhonebook::DeleteAllGroupsL()
    {
    LOGSTRING("Creator: CCreatorPhonebook::DeleteAllGroupsL");
    iPhonebookWrapper->DeleteAllGroupsL();
    }

//----------------------------------------------------------------------------
void CCreatorPhonebook::DeleteAllGroupsCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorPhonebook::DeleteAllGroupsCreatedByCreatorL");
    DeleteItemsCreatedWithCreatorL( KUidDictionaryUidContactGroups );
    }

//----------------------------------------------------------------------------
void CCreatorPhonebook::DeleteContactsL( RArray<TUint32>& aContactsToDelete, TUid aStoreUid	)
    {
	iPhonebookWrapper->DeleteContactsL( aContactsToDelete, aStoreUid );
    }

//----------------------------------------------------------------------------


void CCreatorPhonebook::DeleteItemsCreatedWithCreatorL( TUid aStoreUid )
    {
	CDictionaryFileStore* store = iEngine->FileStoreLC();
    User::LeaveIfNull( store );
    
    RArray<TUint32> contacts; 
    // backup previous contact links from store
    // otherwise they would be overwritten when calling out.WriteL
    TUint32 creatorLink;
    if ( store->IsPresentL( aStoreUid ) )
       {
       RDictionaryReadStream in;
       in.OpenLC( *store, aStoreUid );
       TRAP_IGNORE( 
           do{ 
               creatorLink = in.ReadUint32L();
               contacts.AppendL( creatorLink );
           }while( creatorLink );
       );
       
       CleanupStack::PopAndDestroy(); // in
       }
    
    
    iPhonebookWrapper->DeleteContactsL( contacts, aStoreUid );
    
    store->Remove( aStoreUid );
    store->CommitL();
    
    CleanupStack::PopAndDestroy( store );
    }

//----------------------------------------------------------------------------
void CCreatorPhonebook::DoDeleteItemsCreatedWithCreatorL( TUid /*aStoreUid*/, CDictionaryFileStore* /*aStore*/ )
    {
    
    }

//----------------------------------------------------------------------------
TBool CCreatorPhonebook::HasOtherThanGroupsL()
    {
    LOGSTRING("Creator: CCreatorPhonebook::HasOtherThanGroupsL");
    TBool result( EFalse );
    return result;
    }


//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
void CCreatorPhonebook::StoreLinksForDeleteL( RArray<TUint32>& aLinks, TUid aStoreUid )
    {
    LOGSTRING("Creator: CCreatorPhonebook::StoreLinksForDeleteL");
    CDictionaryFileStore* store = iEngine->FileStoreLC();
       User::LeaveIfNull( store );
       
       // backup previous contact links from store
       // otherwise they would be overwritten when calling out.WriteL
       TUint32 previousLink;
       if ( store->IsPresentL( aStoreUid ) )
           {
           RDictionaryReadStream in;
           in.OpenLC( *store, aStoreUid );
           bool r = false;
           TRAP_IGNORE( 
               do{ //iContactMngr->CreateLinksLC( in )) != NULL ) // will leave with KErrEof
                   previousLink = in.ReadUint32L();
                   iPreviousDeleteLinks.AppendL( previousLink );
                   if(previousLink)
                	   {
					   r=true;
                	   }
                   else
                	   {
					   r=false;
                	   }
               }while(r==true); );
           
           CleanupStack::PopAndDestroy(); // in
           }

       RDictionaryWriteStream out;       
       out.AssignLC( *store, aStoreUid );
       
       // restore previous links
       for ( TInt i = 0; i < iPreviousDeleteLinks.Count(); i++ )
           {
           out.WriteUint32L( iPreviousDeleteLinks[i] );
           }
       
       // write new links
       for(int i=0; i < aLinks.Count(); i++)
    	   {
		   out.WriteUint32L( aLinks[i] );
    	   }
       out.CommitL();
       CleanupStack::PopAndDestroy(); // out
       
       store->CommitL();

       CleanupStack::PopAndDestroy( store );
    }

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void CCreatorPhonebook::InitializeContactParamsL()
    {
    LOGSTRING("Creator: CCreatorPhonebook::InitializeContactParamsL");        
 
    iParameters = new (ELeave) CPhonebookParameters;
           
    iParameters->iNumberOfPhoneNumberFields = iNumberOfPhoneNumberFields;
    iParameters->iNumberOfURLFields = iNumberOfURLFields;
    iParameters->iNumberOfEmailAddressFields = iNumberOfEmailAddressFields;       
    
    TPtrC fname = iEngine->RandomString(CCreatorEngine::EFirstName);
    TPtrC lname = iEngine->RandomString(CCreatorEngine::ESurname);
    
    
    if( iAddAllFields )
        {
		TInt textFieldCount = sizeof(CreatorPbkMiscTextFields) / sizeof(DetailFieldInfo);
		for( TInt tfIndex = 0; tfIndex < textFieldCount; ++tfIndex )
			{ 
			CCreatorContactField* field = CCreatorContactField::NewL();
			CleanupStack::PushL(field);
	        field->AddFieldToParamL( iEngine,iParameters, CreatorPbkMiscTextFields[tfIndex].iFieldCode, CreatorPbkMiscTextFields[tfIndex].iRandomType );
  	        CleanupStack::Pop(field);
            }
		
		// Add binary fields:
		
		RFs& fs = CCoeEnv::Static()->FsSession();
		      
        TBuf<KMaxFileName> srcPath;
		iEngine->RandomPictureFileL(srcPath);
		TBuf<KMaxFileName> destPath(KTempPath);
		                            
		if(!BaflUtils::FolderExists( fs, destPath ))
			{
		    BaflUtils::EnsurePathExistsL( fs, destPath );
		    }
		                
		TInt err=BaflUtils::CopyFile( fs, srcPath, destPath );

		TParse temp;
		temp.Set( srcPath,NULL,NULL );
		destPath.Append(temp.NameAndExt());
		TPtrC picture;
		picture.Set(destPath);
		CCreatorContactField* picturefield = CCreatorContactField::NewL();
		CleanupStack::PushL(picturefield);
		picturefield->AddFieldToParamL( iParameters, CCreatorPhonebookWrapper::EThumbnailPic, picture );
		CleanupStack::Pop(picturefield);
		// Add date-time fields:
		TPtrC anniv;
		TBuf<20> timeString;
		TTime datetime = iEngine->RandomDate( CCreatorEngine::EDateFuture );
		_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
		TRAP_IGNORE( datetime.FormatL(timeString, KDateString) );
		
		anniv.Set(timeString);
 
		CCreatorContactField* fieldAnniv = CCreatorContactField::NewL();
		CleanupStack::PushL( fieldAnniv );
		fieldAnniv->AddFieldToParamL(iParameters,CCreatorPhonebookWrapper::EAnniversary, anniv );
		CleanupStack::Pop( fieldAnniv );
    //***************************************************************************
    
        }
    else
        {
		CCreatorContactField* field = CCreatorContactField::NewL();
    	CleanupStack::PushL(field);
    	field->AddFieldToParamL( iParameters, CCreatorPhonebookWrapper::EFirstName, fname );
		field->AddFieldToParamL( iParameters, CCreatorPhonebookWrapper::ELastName, lname );
		CleanupStack::Pop(field);
        }
                        
      // Phone numbers:
    TInt phoneFieldCount = sizeof(CreatorPbkPhoneNumberFields) / sizeof(TInt);
    
    TInt inc=0;
    for( TInt i=0; i<iNumberOfPhoneNumberFields; i++ )
    	{
    
		if(inc >= phoneFieldCount )
        	{
    		inc = 0;
        	}
		
		CCreatorContactField* field = CCreatorContactField::NewL();
        CleanupStack::PushL(field);
		field->AddFieldToParamL(iEngine, iParameters, CreatorPbkPhoneNumberFields[inc] );
		CleanupStack::Pop(field);
    
		inc++;
    	}
    
    
    
    // URLs:
    TInt urlFieldCount = sizeof(CreatorPbkUrlFields) / sizeof(TInt);
    inc = 0;
    for( TInt i=0; i<iNumberOfURLFields; i++ )
        {
    
		if(inc >= urlFieldCount )
          	{
        	inc = 0;
           	}
    
		CCreatorContactField* field = CCreatorContactField::NewL();
        CleanupStack::PushL(field);
    	field->AddFieldToParamL(iEngine, iParameters, CreatorPbkUrlFields[inc] );
    	CleanupStack::Pop(field);
    	inc++;
        }

    // EMail addresses:
    TInt emailFieldCount = sizeof(CreatorPbkEmailFields) / sizeof(TInt);
    inc = 0;
    for( int i=0; i<iNumberOfEmailAddressFields; i++ )
    	{
		if(inc >= emailFieldCount )
           	{
          	inc = 0;
           	}
        CCreatorContactField* field = CCreatorContactField::NewL();
        CleanupStack::PushL(field);
        field->AddFieldToParamL(iEngine, iParameters, CreatorPbkEmailFields[inc] );
        CleanupStack::Pop(field);
        inc++;
    	}
    
    TestPrintOut(iParameters);
    
   }

void CCreatorPhonebook::TestPrintOut(CPhonebookParameters* aParam)
	{
	LOGSTRING("Creator: CCreatorPhonebook::TestPrintOut");
	for( TCreatorContactFields::iterator it = aParam->iContactFields.begin(); it != aParam->iContactFields.end(); ++it)
        {
        HBufC* temp = (*it).second;
        if(temp)
        	{
			LOGSTRING3("Type: %d Content:%S ", (*it).first, &temp->Des() );
        	}
        }
    }

// Checks if the link is a group or not
TBool CCreatorPhonebook::IsContactGroupL( TUint32& /*aLink*/ )
	{
	//not used right now
	return EFalse;
	}


TInt CCreatorPhonebook::CreateGroupEntryL(CCreatorModuleBaseParameters *aParameters)
    {
	 LOGSTRING("Creator: CCreatorPhonebook::CreateGroupEntryL");
	 TInt err = KErrNone;
	 
	    delete iParameters;
	    iParameters = 0;
	    
	    CPhonebookParameters* parameters = (CPhonebookParameters*) aParameters;
	    
	    if( !parameters )
	       {
	       iParameters = new (ELeave) CPhonebookParameters;
	       iParameters->iContactsInGroup = iContactsInGroup;
	       iParameters->iGroupName->Des().Copy( iEngine->RandomString(CCreatorEngine::EGroupName) );
	       iParameters->iGroupName->Des().Append( _L(" #") );
	       iParameters->iGroupName->Des().AppendNum( iEngine->RandomNumber(1000, 9999) );
	       parameters = iParameters;
	       }
	   
	    // create a new contact group
	    
	    TUint32 newGroupId = iPhonebookWrapper->CreateGroupEntryL( parameters->iGroupName );
	    
	    // define amounts of contacts to be added to the group
	    TInt amountOfContactsToBeAdded = 0;
	    if (parameters->iContactsInGroup == KCreateRandomAmountOfGroups)
	    	{
	        amountOfContactsToBeAdded = iEngine->RandomNumber(30);
	    	}
	    else 
	    	{
	        amountOfContactsToBeAdded = parameters->iContactsInGroup;
	    	}
	    
	    if( parameters->iLinkIds.Count() > 0 )
	        {
	        for( TInt i = 0; i < parameters->iLinkIds.Count(); ++i )
	            {
	            const CCreatorContactSet& set = ContactLinkCache::Instance()->ContactSet(parameters->iLinkIds[i].iLinkId);
	          
	            const RArray<TUint32> links = set.ContactLinks();//ContactLinkCache::Instance()->ContactSets();//set.ContactLinks();
	            TInt numberOfExplicitLinks = links.Count(); // Number of defined contacts in contact-set
	            TInt numberOfExistingContacts = set.NumberOfExistingContacts(); // Number of existing contacts in contact-set
	            TInt maxAmount = numberOfExplicitLinks + numberOfExistingContacts;
	            
	            if( parameters->iLinkIds[i].iLinkAmount > 0 )
	                maxAmount = parameters->iLinkIds[i].iLinkAmount; // Max amount is limited
	            
	            TInt addedMembers = 0;
	            
	            for( TInt j = 0; j < links.Count() && addedMembers < maxAmount; ++j )
	                {
					
					addedMembers += iPhonebookWrapper->AddContactToGroup(newGroupId,links[j]);
	           
	                }
	            if( addedMembers < maxAmount )
	            	{
	            	// Add existing contacts, withing the limits set by maxAmount:
	            	amountOfContactsToBeAdded += maxAmount - addedMembers;
	            	}
	            }
	        }
	    if( amountOfContactsToBeAdded > 0 )
	        {
			TInt addedContacts = iPhonebookWrapper->AddToGroup(newGroupId, amountOfContactsToBeAdded);
		
	        }
	    
	    // store the link to contact, so that Creator is able to delete
	    // it when user requests deletion of contacts that were created with Creator 
	    iContactGroupsToDelete.AppendL( newGroupId );
	    
	    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorPhonebook::CreateSubscribedContactEntryL(CCreatorModuleBaseParameters* /*aParameters*/)
    {
    LOGSTRING("Creator: CCreatorPhonebook::CreateSubscribedContactEntryL");

    return KErrNotSupported;
    }

//----------------------------------------------------------------------------

CCreatorContactField::CCreatorContactField()
    {
    }
CCreatorContactField::~CCreatorContactField()
    {
    }

CCreatorContactField* CCreatorContactField::NewL()
    {
    CCreatorContactField* self = new (ELeave) CCreatorContactField();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
void CCreatorContactField::ConstructL()
    {
    }


void CCreatorContactField::AddFieldToParamL( CCreatorEngine* aEngine, CPhonebookParameters* aParam, TInt aType, TInt aRand )
    {
    HBufC* content = NULL;
    if( aRand == KErrNotFound ) 
        {
        TInt textFieldCount = sizeof(CreatorPbkPhoneNumberFields)/sizeof(TInt);;
        for( TInt tfIndex = 0; tfIndex < textFieldCount; ++tfIndex )
            {
            if( CreatorPbkPhoneNumberFields[tfIndex] == aType )
                {
                content = aEngine->RandomString(CCreatorEngine::EPhoneNumber).AllocL();
                break;
                }
            }
    
        textFieldCount = sizeof(CreatorPbkUrlFields)/sizeof(TInt);;
        for( TInt tfIndex = 0; tfIndex < textFieldCount; ++tfIndex )
            {
            if( CreatorPbkUrlFields[tfIndex] == aType )
                {
                content = aEngine->CreateHTTPUrlLC(); 
                CleanupStack::Pop();
                break;
                }
            }
        textFieldCount = sizeof(CreatorPbkEmailFields)/sizeof(TInt);;
        for( TInt tfIndex = 0; tfIndex < textFieldCount; ++tfIndex )
            {
            if( CreatorPbkEmailFields[tfIndex] == aType )
                {
                content = aEngine->CreateEmailAddressLC();
                CleanupStack::Pop();
                break;
                }
            }
        
    
        //if there is textfield without specified random number
        textFieldCount = sizeof(CreatorPbkMiscTextFields) / sizeof(DetailFieldInfo);
        for( TInt tfIndex = 0; tfIndex < textFieldCount; ++tfIndex )
            {
			if( CreatorPbkMiscTextFields[tfIndex].iFieldCode == aType && CreatorPbkMiscTextFields[tfIndex].iRandomType != RND_TYPE_UNDEF )
            	{
				TInt rand =  CreatorPbkMiscTextFields[tfIndex].iRandomType;
				content = aEngine->RandomString((CCreatorEngine::TRandomStringType) rand).AllocL();
				break;
				}
			}	
        }

    if( aRand != KErrNotFound && aRand != RND_TYPE_UNDEF ) 
        {
        TInt textFieldCount = sizeof(CreatorPbkMiscTextFields) / sizeof(DetailFieldInfo);
        for( TInt tfIndex = 0; tfIndex < textFieldCount; ++tfIndex )
            {
            if( CreatorPbkMiscTextFields[tfIndex].iFieldCode == aType )
                {
                content = aEngine->RandomString((CCreatorEngine::TRandomStringType) aRand).AllocL();
                }
            }
        }
	
	aParam->iContactFields.insert( TCreatorContactField(aType, content) );
	}

void CCreatorContactField::AddFieldToParamL( CPhonebookParameters* aParam, TInt aType, TPtrC aContent )
    {
    aParam->iContactFields.insert( TCreatorContactField(aType, aContent.AllocL()) );
    }

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

