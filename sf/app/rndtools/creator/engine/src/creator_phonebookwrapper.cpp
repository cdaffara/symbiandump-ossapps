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

#include "creator_phonebookwrapper.h" 
#include "creator_traces.h"

#include <bautils.h> 

typedef struct {
TInt iFieldCode;
QString iDetail;
QString iFieldContext;
QString iFieldString;
} QDetailFieldInfo;
//static const TInt RND_TYPE_UNDEF = -99;
QDetailFieldInfo CreatorPbkTextFields[] = {        
        { (TInt)CCreatorPhonebookWrapper::EFirstName,(QContactName::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactName::FieldFirstName).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::ELastName,(QContactName::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactName::FieldLastName).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::ECompanyName,(QContactOrganization::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactOrganization::FieldName).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EJobTitle,(QContactOrganization::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactOrganization::FieldTitle).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EPrefix,(QContactName::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactName::FieldPrefix).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::ESuffix,(QContactName::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactName::FieldSuffix).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EMiddleName,(QContactName::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactName::FieldMiddleName).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrStreetHome,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactAddress::FieldStreet).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrLocalHome,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactAddress::FieldLocality).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrRegionHome,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactAddress::FieldRegion).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrPostCodeHome,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactAddress::FieldPostcode).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrCountryHome,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactAddress::FieldCountry).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrStreetGen,(QContactAddress::DefinitionName).operator QString(), "", (QContactAddress::FieldStreet).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrLocalGen,(QContactAddress::DefinitionName).operator QString(), "", (QContactAddress::FieldLocality).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrRegionGen,(QContactAddress::DefinitionName).operator QString(), "", (QContactAddress::FieldRegion).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrPostCodeGen,(QContactAddress::DefinitionName).operator QString(), "", (QContactAddress::FieldPostcode).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrCountryGen,(QContactAddress::DefinitionName).operator QString(), "", (QContactAddress::FieldCountry).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrStreetWork,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactAddress::FieldStreet).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrLocalWork,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactAddress::FieldLocality).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrRegionWork,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactAddress::FieldRegion).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrPostCodeWork,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactAddress::FieldPostcode).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAddrCountryWork,(QContactAddress::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactAddress::FieldCountry).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EMobilePhoneWork,(QContactPhoneNumber::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactPhoneNumber::SubTypeMobile).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::ELandPhoneWork,(QContactPhoneNumber::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactPhoneNumber::SubTypeLandline).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::ENote,(QContactNote::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactNote::FieldNote).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EDepartment,(QContactOrganization::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactOrganization::FieldDepartment).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EAsstName,(QContactOrganization::DefinitionName).operator QString(), (QContactDetail::ContextWork).operator QString(), (QContactOrganization::FieldAssistantName).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::ESpouse,(QContactFamily::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactFamily::FieldSpouse).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EChildren,(QContactFamily::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(), (QContactFamily::FieldChildren).operator QString()},
        { (TInt)CCreatorPhonebookWrapper::EGenLabel,(QContactName::DefinitionName).operator QString(), (QContactDetail::ContextHome).operator QString(),  (QContactName::FieldCustomLabel).operator QString()}
        };
                  

typedef struct{
TInt iFieldCode;
QString iFieldContext;
QString iFieldString;
}PhoneNumInfo;
PhoneNumInfo CreatorPhoneNumberFields[] =
    {
    { CCreatorPhonebookWrapper::ELandPhoneGen,"", "Landline"},
    { CCreatorPhonebookWrapper::ELandPhoneHome,"Home", "Landline"},
    { CCreatorPhonebookWrapper::ELandPhoneWork,"Work", "Landline"},                        
    { CCreatorPhonebookWrapper::EMobilePhoneGen,"","Mobile"},
    { CCreatorPhonebookWrapper::EMobilePhoneHome,"Home","Mobile"},
    { CCreatorPhonebookWrapper::EMobilePhoneWork,"Work", "Mobile"},
    { CCreatorPhonebookWrapper::EFaxNumberGen, "", "Facsimile"},
    { CCreatorPhonebookWrapper::EFaxNumberHome, "Home", "Facsimile"},
    { CCreatorPhonebookWrapper::EFaxNumberWork, "Work", "Facsimile"},
    { CCreatorPhonebookWrapper::EPagerNumber, "Work", "Pager"},
    { CCreatorPhonebookWrapper::EVideoNumberGen, "", "Video"},
    { CCreatorPhonebookWrapper::EVideoNumberHome, "Home", "Video"},
    { CCreatorPhonebookWrapper::EVideoNumberWork, "Work", "Video"},   //{ "Home", "Voice" },//{ "Work", "Voice" },
    { CCreatorPhonebookWrapper::EAsstPhone, "Work", "Assistant" },
    { CCreatorPhonebookWrapper::ECarPhone, "Home",  "Car" }
    };


typedef struct{
	TInt iFieldCode;
	QString iFieldContext;
}EmailInfo;
EmailInfo CreatorEmailFields[] =
	{
    {CCreatorPhonebookWrapper::EEmailGen,""},
    {CCreatorPhonebookWrapper::EEmailHome,(QContactDetail::ContextHome).operator QString()},
    {CCreatorPhonebookWrapper::EEmailWork,(QContactDetail::ContextWork).operator QString()}
    };

typedef struct{
	TInt iFieldCode;
	QString iFieldContext;
}UrlInfo;
UrlInfo CreatorUrlFields[] =
    {
    {CCreatorPhonebookWrapper::EUrlGen,""},
    {CCreatorPhonebookWrapper::EUrlHome,(QContactDetail::ContextHome).operator QString()},
    {CCreatorPhonebookWrapper::EUrlWork,(QContactDetail::ContextWork).operator QString()}
    };

CCreatorPhonebookWrapper* CCreatorPhonebookWrapper::NewL()
    {
    CCreatorPhonebookWrapper* self = CCreatorPhonebookWrapper::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CCreatorPhonebookWrapper* CCreatorPhonebookWrapper::NewLC()
    {
    CCreatorPhonebookWrapper* self = new (ELeave) CCreatorPhonebookWrapper();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


CCreatorPhonebookWrapper::CCreatorPhonebookWrapper() 
    {
    }

void CCreatorPhonebookWrapper::ConstructL()
    {
    LOGSTRING("Creator: CCreatorPhonebookWrapper::ConstructL");

    QT_TRYCATCH_LEAVING( iPhonebookAPI = new CCreatorPhonebookAPI() );
	
	}
	

CCreatorPhonebookWrapper::~CCreatorPhonebookWrapper()
    {
    LOGSTRING("Creator: CCreatorPhonebookWrapper::~CCreatorPhonebookWrapper");
	if( iPhonebookAPI )
        {
        delete iPhonebookAPI;
        iPhonebookAPI = NULL;
        }
    }

//----------------------------------------------------------------------------

QList<QContactDetail> CCreatorPhonebookWrapper::CreateContactDetailsFromParameters( const TCreatorContactFields& aFields )
	{
	QList<QContactDetail> contDetList;
	QString content;
	HBufC* temp;
	TInt arraySize = sizeof(CreatorPbkTextFields)/sizeof(QDetailFieldInfo);
	for (TInt i = 0; i < arraySize; i++)
		{
	    TCreatorContactFields::const_iterator it = aFields.find(CreatorPbkTextFields[i].iFieldCode);
		if( it != aFields.end() )
			{
		    temp = (*it).second;
			content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
			QContactDetail contactDetail =  CreateContactDetail(contDetList, CreatorPbkTextFields[i].iDetail,CreatorPbkTextFields[i].iFieldContext,CreatorPbkTextFields[i].iFieldString, content );
			AddFieldToList( contDetList, contactDetail );
			}
		}
	arraySize = sizeof(CreatorPhoneNumberFields)/sizeof(PhoneNumInfo);
		for (TInt i = 0; i < arraySize; i++)
			{
			TCreatorContactFields::const_iterator it = aFields.find(CreatorPhoneNumberFields[i].iFieldCode);
			if( it != aFields.end() )
				{
			    temp = (*it).second;
				content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
				QContactDetail contactDetail =  CreateContactDetail(contDetList, QContactPhoneNumber::DefinitionName ,CreatorPhoneNumberFields[i].iFieldContext,CreatorPhoneNumberFields[i].iFieldString, content );
				AddFieldToList( contDetList, contactDetail );
				}
			}
		
	arraySize = sizeof(CreatorEmailFields)/sizeof(EmailInfo);
		for (TInt i = 0; i < arraySize; i++)
			{
			TCreatorContactFields::const_iterator it = aFields.find(CreatorEmailFields[i].iFieldCode);
			if( it != aFields.end() )
				{
			    temp = (*it).second;
				content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
				QContactDetail contactDetail =  CreateContactDetail(contDetList, QContactEmailAddress::DefinitionName ,CreatorEmailFields[i].iFieldContext,"", content );
				AddFieldToList( contDetList, contactDetail );
				}
			}
		
	arraySize = sizeof(CreatorUrlFields)/sizeof(UrlInfo);
			for (TInt i = 0; i < arraySize; i++)
				{
				TCreatorContactFields::const_iterator it = aFields.find(CreatorUrlFields[i].iFieldCode);
				if( it != aFields.end() )
					{
				    temp = (*it).second;
					content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
					QContactDetail contactDetail =  CreateContactDetail(contDetList, QContactUrl::DefinitionName ,CreatorUrlFields[i].iFieldContext,"", content );
					AddFieldToList( contDetList, contactDetail );
					}
				}	
	arraySize = sizeof(CreatorUrlFields)/sizeof(UrlInfo);
			for (TInt i = 0; i < arraySize; i++)
				{
				TCreatorContactFields::const_iterator it = aFields.find(CreatorUrlFields[i].iFieldCode);
				if( it != aFields.end() )
					{
					temp = (*it).second;
					content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
					QContactDetail contactDetail =  CreateContactDetail(contDetList, QContactUrl::DefinitionName ,CreatorUrlFields[i].iFieldContext,"", content );
					AddFieldToList( contDetList, contactDetail );
					}
				}
			
			TCreatorContactFields::const_iterator it = aFields.find(CCreatorPhonebookWrapper::EThumbnailPic);
			if( it != aFields.end() )
				{
				temp = (*it).second;
				if(temp)
					{
					content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
					QContactDetail contactDetail =  CreateContactDetail(contDetList, QContactAvatar::DefinitionName ,"","", content );
					AddFieldToList( contDetList, contactDetail );
					}
				}
			it = aFields.find(CCreatorPhonebookWrapper::EAnniversary);
				if( it != aFields.end() )
					{
					temp = (*it).second;
					if(temp)
						{
						content = QString::fromUtf16( temp->Des().Ptr(), temp->Length() );
						QContactDetail contactDetail =  CreateContactDetail(contDetList, QContactAnniversary::DefinitionName ,"","", content );
						AddFieldToList( contDetList, contactDetail );
						}
					}
		
	return contDetList;
	}
QContactDetail CCreatorPhonebookWrapper::CreateContactDetail( QList<QContactDetail>& aContactDetailList,QString aDetail, QString aFieldContext, QString aFieldString, QString aData )
    {
    QContactDetail contactDetail;
        
        if( aDetail == QContactPhoneNumber::DefinitionName)
            {
            QContactPhoneNumber phoneNumber;// = contactDetail;
            if(!aFieldContext.isEmpty())
            	{
				phoneNumber.setContexts(aFieldContext);
            	}
            phoneNumber.setSubTypes(aFieldString);
            //QString number = QString::fromUtf16(aData.Ptr(),aData.Length());
            phoneNumber.setNumber(aData);
            return phoneNumber;
            }
        else if( aDetail == QContactName::DefinitionName )          //--Contact NAME-----------------------------
            {
            QContactName contactName;
            for(int i = 0 ; i < aContactDetailList.count() ; i++ ) //go through all contact details to check if there is already Contact Name to set other details
                {
                if(aContactDetailList.at(i).definitionName() == QContactName::DefinitionName )
                    {
                    contactName = aContactDetailList.at(i);
                    }
                }
            //QString name = QString::fromUtf16(aData.Ptr(),aData.Length());
            if(aFieldString == QContactName::FieldFirstName)
                {
                if(contactName.firstName().isEmpty())
                    {
                    contactName.setFirstName( aData );
                    }
                }
            else if(aFieldString == QContactName::FieldLastName)
                {
                if(contactName.lastName().isEmpty())
                    {
                    contactName.setLastName( aData );
                    }
                }
            else if(aFieldString == QContactName::FieldMiddleName)
                {
                if(contactName.middleName().isEmpty())
                    {
                    contactName.setMiddleName( aData );
                    }
                }
            else if(aFieldString == QContactName::FieldPrefix)
                {
                if(contactName.prefix().isEmpty())
                    {
                    contactName.setPrefix( aData );
                    }
                }
            else if(aFieldString == QContactName::FieldSuffix)
                {
                if(contactName.suffix().isEmpty())
                    {
                    contactName.setSuffix( aData );
                    }
                }
            else        //QContactName::FieldCustomLabel:
                {
                if(contactName.customLabel().isEmpty())
                    {
                    contactName.setCustomLabel( aData );
                    }
                }
            return contactName;
            }
        else if( aDetail == QContactOrganization::DefinitionName )          //--Contact Company-----------------------------
            {
            QContactOrganization contactCompany;
            
            for(int i = 0 ; i < aContactDetailList.count() ; i++ ) //go through all contact details to check if there is already Contact Name to set other details
                {
                if(aContactDetailList.at(i).definitionName() == QContactOrganization::DefinitionName )
                    {
                    contactCompany = aContactDetailList.at(i);
                    }
                }
            
            //QString company = QString::fromUtf16(aData.Ptr(),aData.Length());
            if(aFieldString == QContactOrganization::FieldName)
                {
                if(contactCompany.name().isEmpty())
                    {
                    contactCompany.setName( aData );
                    }
                }
            if(aFieldString == QContactOrganization::FieldTitle)
                {
                if(contactCompany.title().isEmpty())
                    {
                    contactCompany.setTitle( aData );
                    }
                }
            if(aFieldString == QContactOrganization::FieldDepartment)
                {
                QStringList depList = contactCompany.department();
                depList.append(aData);
                contactCompany.setDepartment(depList);
                }
            if(aFieldString == QContactOrganization::FieldAssistantName)
                {
                if(contactCompany.assistantName().isEmpty())
                    {
                    contactCompany.setAssistantName( aData );
                    }
                }
            return contactCompany;
            }
        else if( aDetail == QContactAddress::DefinitionName )           //--Contact Address-----------------------------
                {
                QContactAddress contactAddress;
                
                for(int i = 0 ; i < aContactDetailList.count() ; i++ ) //go through all contact details to check if there is already Contact Name to set other details
                    {
                    if(aContactDetailList.at(i).definitionName() == QContactAddress::DefinitionName && aContactDetailList.at(i).value(QContactDetail::FieldContext) == aFieldContext )
                        {
                        contactAddress = aContactDetailList.at(i);
                        }
                    }
                if( !aFieldContext.isEmpty() )
                    {
					contactAddress.setContexts( aFieldContext );
                    }
                //QString address = QString::fromUtf16(aData.Ptr(),aData.Length());
                if(aFieldString == QContactAddress::FieldStreet )
                    {
                    if( contactAddress.street().isEmpty() )
                        {
                        contactAddress.setStreet( aData );
                        }
                    }
                else if(aFieldString == QContactAddress::FieldLocality )
                    {
                    if( contactAddress.locality().isEmpty() )
                        {
                        contactAddress.setLocality( aData );
                        }
                    }
                else if(aFieldString == QContactAddress::FieldRegion )
                    {
                    if( contactAddress.region().isEmpty() )
                        {
                        contactAddress.setRegion( aData );
                        }
                    }
                else if(aFieldString == QContactAddress::FieldPostcode )
                    {
                    if( contactAddress.postcode().isEmpty() )
                        {
                        contactAddress.setPostcode( aData );
                        }
                    }
                else if(aFieldString == QContactAddress::FieldCountry )
                    {
                    if( contactAddress.country().isEmpty() )
                        {
                        contactAddress.setCountry( aData );
                        }
                    }
                else 
                    {
                    return contactDetail;
                    }
                return contactAddress;
                }
        else if( aDetail == QContactNote::DefinitionName )          //--Contact Note-----------------------------
                    {
                    QContactNote contactNote;
                    //QString note = QString::fromUtf16(aData.Ptr(),aData.Length());
                    contactNote.setNote(aData);
                    return contactNote;
                    }
        else if( aDetail == QContactFamily::DefinitionName )            //--Contact Family-----------------------------
                    {
                    QContactFamily contactFamily;
                    
                    for(int i = 0 ; i < aContactDetailList.count() ; i++ ) //go through all contact details to check if there is already Contact Name to set other details
                        {
                        if(aContactDetailList.at(i).definitionName() == QContactFamily::DefinitionName && aContactDetailList.at(i).value(QContactDetail::FieldContext) == aFieldContext )
                            {
                            contactFamily = aContactDetailList.at(i);
                            }
                        }
                   
                    //QString familyData = QString::fromUtf16(aData.Ptr(),aData.Length());
                    if(aFieldString == QContactFamily::FieldSpouse )
                        {
                        if( contactFamily.spouse().isEmpty() )
                            {
                            contactFamily.setSpouse( aData );
                            }
                        }
                    if(aFieldString == QContactFamily::FieldChildren )
                        {
                            QStringList children = contactFamily.children();
                            children.append( aData );
                            contactFamily.setChildren( children );
                        }
                    
                    return contactFamily;
                    }
        
        if( aDetail == QContactAvatar::DefinitionName)                      //--Contact Picture-----------------------------
                {
                QContactAvatar contactAvatar;
                QUrl imageUrl;
                imageUrl.setUrl(aData);
                contactAvatar.setImageUrl(imageUrl);

                return contactAvatar;
                }
        if( aDetail == QContactAnniversary::DefinitionName) //--Anniversary------------------------------
                {
                QContactAnniversary contactAnniversary;
                aData.replace(QChar('/'), QChar('-'));
       		    QDate date = QDate::fromString(aData, "dd-MM-yyyy");
                //TTime datetime = aEngine->RandomDate( CCreatorEngine::EDateFuture );
                //date.setDate( datetime.DateTime().Year(),(int) (datetime.DateTime().Month()+1), datetime.DateTime().Day() );
                contactAnniversary.setOriginalDate( date );
                return contactAnniversary;
                }
        if( aDetail == QContactEmailAddress::DefinitionName) //--Email------------------------------------
            {
            QContactEmailAddress email;
            //QString emailAddr = QString::fromUtf16(aData.Ptr(),aData.Length());
            if( !aFieldContext.isEmpty() )
            	{
				email.setContexts( aFieldContext );
            	}
            email.setEmailAddress( aData );
            return email;
            }
        if( aDetail == QContactUrl::DefinitionName )    //--Url-------------------------------------------
            {
            QContactUrl url;
            //QString urlStr = QString::fromUtf16(aData.Ptr(),aData.Length());
            if( !aFieldContext.isEmpty() )
                {
				url.setContexts( aFieldContext );
                }
            url.setUrl(aData);
            return url;
            }
        if( aDetail == QContactBirthday::DefinitionName )   //--Birthday-----------------------------------
            {
            QContactBirthday birthday;
            QDate date = QDate::fromString(aData);
            //TTime datetime = aEngine->RandomDate( CCreatorEngine::EDatePast );
            //date.setDate( datetime.DateTime().Year(),(int) (datetime.DateTime().Month()+1), datetime.DateTime().Day() );
            birthday.setDate( date );
            return birthday;
            }
                
        return contactDetail;
    }

void CCreatorPhonebookWrapper::AddFieldToList( QList<QContactDetail>& aDetailList, QContactDetail aDetail)
    {
     bool replace = false;
     for(int i = 0 ; i< aDetailList.count() ; i++) //go through
          {
           if( !aDetail.isEmpty() && aDetail.definitionName() == aDetailList.at(i).definitionName() 
        		   && aDetail.definitionName() != QContactPhoneNumber::DefinitionName 
        		   && aDetail.definitionName() != QContactEmailAddress::DefinitionName
        		   && aDetail.definitionName() != QContactUrl::DefinitionName )
               {
               QString context = aDetail.value(QContactDetail::FieldContext);
               bool isContextEmpty = context.isEmpty();
               if( isContextEmpty ||  ( aDetail.value(QContactDetail::FieldContext) == aDetailList.at(i).value(QContactDetail::FieldContext)) )
                   {
                   //replace
                   aDetailList.replace(i,aDetail);
                   replace = true;
                   }
               }
          }
          if(!replace)
             {
             if(!aDetail.isEmpty())
                {
                aDetailList.append(aDetail);
                }
             }
      
    }

TUint32 CCreatorPhonebookWrapper::CreateContactEntryL(const TCreatorContactFields& aFields)
    {
	QList<QContactDetail> list = CreateContactDetailsFromParameters( aFields );
	
	return iPhonebookAPI->saveContact( list );
    }


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

void CCreatorPhonebookWrapper::DeleteAllL()
    {
	iPhonebookAPI->deleteAllContacts();
    }

//----------------------------------------------------------------------------
void CCreatorPhonebookWrapper::DeleteAllGroupsL()
    {
    LOGSTRING("Creator: CCreatorPhonebookWrapper::DeleteAllGroupsL");
    
    iPhonebookAPI->deleteAllContacts( QContactType::TypeGroup );

    }

//----------------------------------------------------------------------------
void CCreatorPhonebookWrapper::DeleteContactsL( RArray<TUint32>& aContactsToDelete, TUid aStoreUid )
    {
	QList<QContactLocalId> contacts; 
	for( TInt i = 0; i < aContactsToDelete.Count(); ++i )
	    {
	    QContact contact = iPhonebookAPI->contact( QContactLocalId( aContactsToDelete[i] ) );
	    if( (contact.type() == QContactType::TypeGroup && aStoreUid == KUidDictionaryUidContactGroups ) ||  (contact.type() != QContactType::TypeGroup && aStoreUid != KUidDictionaryUidContactGroups) )
            {
            contacts.append( QContactLocalId( aContactsToDelete[i] ) );
            }
	    }
    QMap<int, QContactManager::Error> errorMap;
	iPhonebookAPI->deleteContacts( contacts );
    }

//----------------------------------------------------------------------------
TBool CCreatorPhonebookWrapper::HasOtherThanGroupsL()
    {
    LOGSTRING("Creator: CCreatorPhonebookWrapper::HasOtherThanGroupsL");
    TBool result( EFalse );
    return result;
    }


TUint32 CCreatorPhonebookWrapper::CreateGroupEntryL( HBufC* aGroupName )
    {
	TUint32 id = 0;
	QString groupName = QString::fromUtf16( aGroupName->Des().Ptr(), aGroupName->Length() );
	
	id = iPhonebookAPI->createGroup( groupName );
	
	return id; 
    }
TInt CCreatorPhonebookWrapper::NumberOfContacts()
	{
	//return all stored contacts
	return iPhonebookAPI->numberOfContacts();
	}

TInt CCreatorPhonebookWrapper::AddContactToGroup( TUint32 aGroupId, TUint32 aContact )
	{
	QContactLocalId group = QContactLocalId(aGroupId);
	QContactLocalId contact = QContactLocalId(aContact);
	
	TInt ret = iPhonebookAPI->addContactToGroup(group, contact);

	return ret;
	}
TInt CCreatorPhonebookWrapper::AddToGroup( TUint32 aGroupId, TInt aAmount )
	{
	QContactLocalId group = QContactLocalId(aGroupId);
	TInt ret = iPhonebookAPI->addToGroup( group, (int) aAmount );
	return ret;
	}


//----------------------------------------------------------------------------

TInt CCreatorPhonebookWrapper::CreateSubscribedContactEntryL()
    {
    LOGSTRING("Creator: CCreatorPhonebookWrapper::CreateSubscribedContactEntryL");

    return KErrNotSupported;
    }


HBufC* CCreatorPhonebookWrapper::GetPhoneNumberL( TUint32 aContactId )
    {
    QContactLocalId contact = QContactLocalId(aContactId);
    QString phoneNumber = iPhonebookAPI->phoneNumber(contact);
    HBufC *buf = HBufC::NewLC( phoneNumber.length() );
    buf->Des().Copy( phoneNumber.utf16() );
    CleanupStack::Pop( buf );
    return buf;
    }

TBool CCreatorPhonebookWrapper::GetContactDetailsL( TUint32 aContactId, TDes& aName, TDes& aPhoneNumber, TDes& aEmail )
    {
    QString name;
    QString phoneNumber;
    QString email;
    
    QContactLocalId contact = QContactLocalId(aContactId);
    bool success = iPhonebookAPI->contactDetails( contact, name, phoneNumber, email);
    if( success )
        {
        aName.Copy( name.utf16() );
        aPhoneNumber.Copy( phoneNumber.utf16() );
        aEmail.Copy( email.utf16() );
        }
    return success ? ETrue : EFalse ;
    }
