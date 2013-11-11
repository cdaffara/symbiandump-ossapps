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



#include "creator_contactelement.h"
#include "creator_traces.h"
#include "creator_phonebook.h"
#include "creator_factory.h"
#include "creator_contactsetcache.h"
#include <xml/documentparameters.h>
#include <qtcontacts.h>

using namespace Xml;
using namespace creatorcontact;

/**
 * Contact field element mappings:
 */
enum TContactFieldDataType
    {
    EDataTypeText,
    EDataTypeBinary,
    EDataTypeDateTime    
    };
    
class FieldMapping
{
public:
    TPtrC iElementName;
    TInt  iFieldCode;
    TContactFieldDataType iDataType;
};
FieldMapping fieldMappingTbl[] = {
       {TPtrC(KFirstname), CCreatorPhonebookWrapper::EFirstName, EDataTypeText},
       {TPtrC(KLastname), CCreatorPhonebookWrapper::ELastName, EDataTypeText},
       {TPtrC(KFirstnameReading), CCreatorPhonebookWrapper::EFirstNameReading, EDataTypeText},
       {TPtrC(KLastnameReading), CCreatorPhonebookWrapper::ELastNameReading, EDataTypeText},
       {TPtrC(KPrefix), CCreatorPhonebookWrapper::EPrefix, EDataTypeText},
       {TPtrC(KSuffix), CCreatorPhonebookWrapper::ESuffix, EDataTypeText},
       {TPtrC(KSecondname), CCreatorPhonebookWrapper::ESecondName, EDataTypeText},
       {TPtrC(KLandphoneHome), CCreatorPhonebookWrapper::ELandPhoneHome, EDataTypeText},
       
       {TPtrC(KMobilephoneHome), CCreatorPhonebookWrapper::EMobilePhoneHome, EDataTypeText},
       {TPtrC(KVideonumberHome), CCreatorPhonebookWrapper::EVideoNumberHome, EDataTypeText},
       {TPtrC(KFaxnumberHome), CCreatorPhonebookWrapper::EFaxNumberHome, EDataTypeText},
       {TPtrC(KVoipHome), CCreatorPhonebookWrapper::EVoipHome, EDataTypeText},
       {TPtrC(KEmailHome), CCreatorPhonebookWrapper::EEmailHome, EDataTypeText},
       {TPtrC(KUrlHome), CCreatorPhonebookWrapper::EUrlHome, EDataTypeText},
       {TPtrC(KAddrlabelHome), CCreatorPhonebookWrapper::EAddrLabelHome, EDataTypeText},
       {TPtrC(KAddrpoHome), CCreatorPhonebookWrapper::EAddrPoHome, EDataTypeText},
       
       {TPtrC(KAddrextHome), CCreatorPhonebookWrapper::EAddrExtHome, EDataTypeText},
       {TPtrC(KAddrstreetHome), CCreatorPhonebookWrapper::EAddrStreetHome, EDataTypeText},
       {TPtrC(KAddrlocalHome), CCreatorPhonebookWrapper::EAddrLocalHome, EDataTypeText},
       {TPtrC(KAddrregionHome), CCreatorPhonebookWrapper::EAddrRegionHome, EDataTypeText},
       {TPtrC(KAddrpostcodeHome), CCreatorPhonebookWrapper::EAddrPostCodeHome, EDataTypeText},
       {TPtrC(KAddrcountryHome), CCreatorPhonebookWrapper::EAddrCountryHome, EDataTypeText},
       {TPtrC(KJobtitle), CCreatorPhonebookWrapper::EJobTitle, EDataTypeText},
       {TPtrC(KCompanyname), CCreatorPhonebookWrapper::ECompanyName, EDataTypeText},
       
       {TPtrC(KLandphoneWork), CCreatorPhonebookWrapper::ELandPhoneWork, EDataTypeText},
       {TPtrC(KMobilephoneWork), CCreatorPhonebookWrapper::EMobilePhoneWork, EDataTypeText},
       {TPtrC(KVideonumberWork), CCreatorPhonebookWrapper::EVideoNumberWork, EDataTypeText},
       {TPtrC(KFaxnumberWork), CCreatorPhonebookWrapper::EFaxNumberWork, EDataTypeText},
       {TPtrC(KVoipWork), CCreatorPhonebookWrapper::EVoipWork, EDataTypeText},
       {TPtrC(KEmailWork), CCreatorPhonebookWrapper::EEmailWork, EDataTypeText},
       {TPtrC(KUrlWork), CCreatorPhonebookWrapper::EUrlWork, EDataTypeText},
       {TPtrC(KAddrlabelWork), CCreatorPhonebookWrapper::EAddrLabelWork, EDataTypeText},
       
       {TPtrC(KAddrpoWork), CCreatorPhonebookWrapper::EAddrPoWork, EDataTypeText},
       {TPtrC(KAddrextWork), CCreatorPhonebookWrapper::EAddrExtWork, EDataTypeText},
       {TPtrC(KAddrstreetWork), CCreatorPhonebookWrapper::EAddrStreetWork, EDataTypeText},
       {TPtrC(KAddrlocalWork), CCreatorPhonebookWrapper::EAddrLocalWork, EDataTypeText},
       {TPtrC(KAddrregionWork), CCreatorPhonebookWrapper::EAddrRegionWork, EDataTypeText},
       {TPtrC(KAddrpostcodeWork), CCreatorPhonebookWrapper::EAddrPostCodeWork, EDataTypeText},
       {TPtrC(KAddrcountryWork), CCreatorPhonebookWrapper::EAddrCountryWork, EDataTypeText},
       {TPtrC(KLandphoneGen), CCreatorPhonebookWrapper::ELandPhoneGen, EDataTypeText},
       
       {TPtrC(KMobilephoneGen), CCreatorPhonebookWrapper::EMobilePhoneGen, EDataTypeText},
       {TPtrC(KVideonumberGen), CCreatorPhonebookWrapper::EVideoNumberGen, EDataTypeText},
       {TPtrC(KFaxnumberGen), CCreatorPhonebookWrapper::EFaxNumberGen, EDataTypeText},
       {TPtrC(KVoipGen), CCreatorPhonebookWrapper::EVoipGen, EDataTypeText},
       {TPtrC(KPoc), CCreatorPhonebookWrapper::EPoc, EDataTypeText},
       {TPtrC(KSwis), CCreatorPhonebookWrapper::ESwis, EDataTypeText},
       {TPtrC(KSip), CCreatorPhonebookWrapper::ESip, EDataTypeText},
       {TPtrC(KEmailGen), CCreatorPhonebookWrapper::EEmailGen, EDataTypeText},
       
       {TPtrC(KUrlGen), CCreatorPhonebookWrapper::EUrlGen, EDataTypeText},
       {TPtrC(KAddrlabelGen), CCreatorPhonebookWrapper::EAddrLabelGen, EDataTypeText},
       {TPtrC(KAddrpoGen), CCreatorPhonebookWrapper::EAddrPoGen, EDataTypeText},
       {TPtrC(KAddrextGen), CCreatorPhonebookWrapper::EAddrExtGen, EDataTypeText},
       {TPtrC(KAddrstreetGen), CCreatorPhonebookWrapper::EAddrStreetGen, EDataTypeText},
       {TPtrC(KAddrlocalGen), CCreatorPhonebookWrapper::EAddrLocalGen, EDataTypeText},
       {TPtrC(KAddrregionGen), CCreatorPhonebookWrapper::EAddrRegionGen, EDataTypeText},
       {TPtrC(KAddrpostcodeGen), CCreatorPhonebookWrapper::EAddrPostCodeGen, EDataTypeText},
       
       {TPtrC(KAddrcountryGen), CCreatorPhonebookWrapper::EAddrCountryGen, EDataTypeText},
       {TPtrC(KPagerNumber), CCreatorPhonebookWrapper::EPagerNumber, EDataTypeText},
       {TPtrC(KDtmfString), CCreatorPhonebookWrapper::EDtmfString, EDataTypeText},
       {TPtrC(KWvAddress), CCreatorPhonebookWrapper::EWVAddress, EDataTypeText},
       {TPtrC(KDate), CCreatorPhonebookWrapper::EDate, EDataTypeDateTime},
       {TPtrC(KNote), CCreatorPhonebookWrapper::ENote, EDataTypeText},
       {TPtrC(KThumbnailPath), CCreatorPhonebookWrapper::EThumbnailPic, EDataTypeText},
       {TPtrC(KThumbnailId), CCreatorPhonebookWrapper::EThumbnailPic, EDataTypeText},
       {TPtrC(KRingTone), CCreatorPhonebookWrapper::ERingTone, EDataTypeText},
       {TPtrC(KRingToneId), CCreatorPhonebookWrapper::ERingTone, EDataTypeText},
       
       {TPtrC(KCallerobjImg), CCreatorPhonebookWrapper::ECallerObjImg, EDataTypeText},
       {TPtrC(KCallerobjText), CCreatorPhonebookWrapper::ECallerObjText, EDataTypeText},
       {TPtrC(KMiddlename), CCreatorPhonebookWrapper::EMiddleName, EDataTypeText},
       {TPtrC(KDepartment), CCreatorPhonebookWrapper::EDepartment, EDataTypeText},
       {TPtrC(KAsstname), CCreatorPhonebookWrapper::EAsstName, EDataTypeText},
       {TPtrC(KSpouse), CCreatorPhonebookWrapper::ESpouse, EDataTypeText},
       {TPtrC(KChildren), CCreatorPhonebookWrapper::EChildren, EDataTypeText},
       {TPtrC(KAsstphone), CCreatorPhonebookWrapper::EAsstPhone, EDataTypeText},
       
       {TPtrC(KCarphone), CCreatorPhonebookWrapper::ECarPhone, EDataTypeText},
       {TPtrC(KAnniversary), CCreatorPhonebookWrapper::EAnniversary, EDataTypeDateTime},
       {TPtrC(KSyncclass), CCreatorPhonebookWrapper::ESyncClass, EDataTypeText},
       {TPtrC(KLocPrivacy), CCreatorPhonebookWrapper::ELocPrivacy, EDataTypeText},
       {TPtrC(KGenlabel), CCreatorPhonebookWrapper::EGenLabel, EDataTypeText}
};

//*********************************************************************************

CCreatorContactElementBase* CCreatorContactElementBase::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    LOGSTRING("Creator: CCreatorContactElementBase::NewL");
    CCreatorContactElementBase* self = new (ELeave) CCreatorContactElementBase(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorContactElementBase::CCreatorContactElementBase(CCreatorEngine* aEngine) : CCreatorScriptElement(aEngine)
    { }

CCreatorContactElement* CCreatorContactElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    LOGSTRING("Creator: CCreatorContactElement::NewL");
    CCreatorContactElement* self = new (ELeave) CCreatorContactElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorContactElement::CCreatorContactElement(CCreatorEngine* aEngine) 
: 
CCreatorContactElementBase(aEngine)
    { 
    iIsCommandElement = ETrue;
    }


void CCreatorContactElement::ExecuteCommandL()
    {    
    LOGSTRING("Creator: CCreatorContactElement::ExecuteCommandL");
    // Find out how many contacts should we create:
    const CCreatorScriptAttribute* contactAmountAttr = this->FindAttributeByName(KAmount);    
    TInt contactAmount = 1;    
    if( contactAmountAttr )
        {
        contactAmount = ConvertStrToIntL(contactAmountAttr->Value());
        }
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    TInt fieldMappingTblSize = sizeof(fieldMappingTbl)/sizeof(FieldMapping);
    for( TInt cI = 0; cI < contactAmount; ++cI )
        {
        CPhonebookParameters* param = (CPhonebookParameters*) TCreatorFactory::CreatePhoneBookParametersL();
        CleanupStack::PushL(param);
        // Loop all the field elements
        if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
            {
            
            const RPointerArray<CCreatorScriptElement>& fieldList = fieldsElement->SubElements();
            
            if( fieldList.Count() == 0 )
                {
                
                }
            for( TInt i = 0; i < fieldList.Count(); ++i )
                {
                TPtrC fieldName = fieldList[i]->Name();
                //const CCreatorScriptAttribute* amountAttr = fieldList[i]->FindAttributeByName(KAmount);
                //const CCreatorScriptAttribute* rndLenAttr = fieldList[i]->FindAttributeByName(KRandomLength);
                //const CCreatorScriptAttribute* increaseAttr = fieldList[i]->FindAttributeByName(KIncrease);
                //TBool increase( EFalse );
                //if ( increaseAttr )
                //    {
                //    increase = ConvertStrToBooleanL( increaseAttr->Value() );
                //    }
                for( TInt j = 0; j < fieldMappingTblSize; ++j )
                    {
                    const FieldMapping& mapping = fieldMappingTbl[j];
                    if( fieldName == mapping.iElementName )
                        {
                        TPtrC content = fieldList[i]->Content();
                                                                        
//                        MCreatorRandomDataField::TRandomLengthType randomLenType = MCreatorRandomDataField::ERandomLengthUndefined;
                    
                        if( content == TPtrC(KEmpty) || content == TPtrC(KNullDesC) )
                            {
							CCreatorContactField* field = CCreatorContactField::NewL();
                        	CleanupStack::PushL(field);
                        	field->AddFieldToParamL( iEngine,param, mapping.iFieldCode, KErrNotFound );
                          	CleanupStack::Pop(field);
                            }
                        else
                        	{
							CCreatorContactField* field = CCreatorContactField::NewL();
                            CleanupStack::PushL(field);
                            field->AddFieldToParamL( param, mapping.iFieldCode, content );
                            CleanupStack::Pop(field);
                        	}
                        

                        break;
                        
                        }
                    }

                }
            } 
        else
            {
            // No fields specified, so add all fields with random content:
            for( TInt i = 0; i < fieldMappingTblSize; ++i )
                {
                if( fieldMappingTbl[i].iElementName == KThumbnailId || 
                    fieldMappingTbl[i].iElementName == KRingToneId )
                    {
                    // Skip thumbnail and ringtone IDs, since they are duplicates with thumbnailpath and ringtonepath
                    continue;
                    }
                CCreatorContactField* field = CCreatorContactField::NewL();
                CleanupStack::PushL(field);
                field->AddFieldToParamL( iEngine,param, fieldMappingTbl[i].iFieldCode, KErrNotFound );
                CleanupStack::Pop(field);
                }
                
            }
        iEngine->AppendToCommandArrayL(ECmdCreatePhoneBookEntryContacts, param);
        iParameters.AppendL(param); // Save the pointer to the parametes. 
        CleanupStack::Pop(param);
        }
    }


/*
void CCreatorContactElement::ExecuteCommandL()
    {}
*/
CCreatorContactSetElement* CCreatorContactSetElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    LOGSTRING("Creator: CCreatorContactSetElement::NewL");
    CCreatorContactSetElement* self = new (ELeave) CCreatorContactSetElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorContactSetElement::CCreatorContactSetElement(CCreatorEngine* aEngine) 
: 
CCreatorContactElementBase(aEngine)
    { }

void CCreatorContactSetElement::ExecuteCommandL()
    { }

void CCreatorContactSetElement::AddToCacheL()
    {    
    LOGSTRING("Creator: CCreatorContactSetElement::AddToCacheL");
    const CCreatorScriptAttribute* linkIdAttr = this->FindAttributeByName(KId);
    const CCreatorScriptAttribute* existingElemsAttr = this->FindAttributeByName(KExistingContacts);
    TInt numOfExistingContacts = 0;
    if( existingElemsAttr )
        {
        numOfExistingContacts = ConvertStrToIntL(existingElemsAttr->Value());
        }
    if( linkIdAttr )
        {
        TInt linkId = ConvertStrToIntL(linkIdAttr->Value());
        if( linkId > 0 )
            {    
            CCreatorContactSet* newSet = CCreatorContactSet::NewL(linkId, numOfExistingContacts);
            CleanupStack::PushL(newSet);
            ContactLinkCache::Instance()->AppendL(newSet);
            CleanupStack::Pop(newSet);
                 
            // Mark sub-elements (i.e. contacts) to this contact-set:
            for( TInt i = 0; i < iSubElements.Count(); ++i )
                {
                for( TInt j = 0; j < iSubElements[i]->CommandParameters().Count(); ++j)
                    {
                    CCreatorModuleBaseParameters* params = iSubElements[i]->CommandParameters()[j];
                    if( params )
                        {
                        params->SetScriptLinkId(linkId);
                        }
                    }
                }
            }
        }
    }

CCreatorContactGroupElement* CCreatorContactGroupElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    LOGSTRING("Creator: CCreatorContactGroupElement::NewL");
    CCreatorContactGroupElement* self = new (ELeave) CCreatorContactGroupElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }
CCreatorContactGroupElement::CCreatorContactGroupElement(CCreatorEngine* aEngine)
: 
CCreatorContactElementBase(aEngine)
    { 
    iIsCommandElement = ETrue;
    }


void CCreatorContactGroupElement::ExecuteCommandL()
    {
    LOGSTRING("Creator: CCreatorContactGroupElement::ExecuteCommandL");
    // Get attributes (amount and name)
    const CCreatorScriptAttribute* groupAmountAttr = this->FindAttributeByName(KAmount);        
    const CCreatorScriptAttribute* groupNameAttr = this->FindAttributeByName(KName);
    // How many groups to create:
    TInt groupAmount = 1;        
    if( groupAmountAttr )
        {
        groupAmount = ConvertStrToIntL(groupAmountAttr->Value());
        }
    
    for( TInt i = 0; i < groupAmount; ++i )
        {        
        CCreatorScriptElement* membersElement = FindSubElement(KMembers);
        
        if( membersElement )
            {    
            CPhonebookParameters* param = (CPhonebookParameters*) TCreatorFactory::CreatePhoneBookParametersL();            
            CleanupStack::PushL(param);
            
            // Links to contact-sets:
            const RPointerArray<CCreatorScriptElement>& linkList = membersElement->SubElements();
            if( linkList.Count() > 0 )
                {
                for( TInt i = 0; i < linkList.Count(); ++i )
                    {
                    AppendContactSetReferenceL(*linkList[i], param->iLinkIds);
                    }                
                }
            
            if( groupNameAttr )
                {
				param->iGroupName->Des().Copy( groupNameAttr->Value() );
                }
            else
            	{
                param->iGroupName->Des().Copy( iEngine->RandomString(CCreatorEngine::EGroupName) );
                }                
            iEngine->AppendToCommandArrayL(ECmdCreatePhoneBookEntryGroups, param);
            CleanupStack::Pop(param);
            }
        }
    }

/*
void CCreatorContactGroupElement::ExecuteCommandL()
    {}
*/
CCreatorContactFieldElement* CCreatorContactFieldElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext)
    {
    LOGSTRING("Creator: CCreatorContactFieldElement::NewL");
    CCreatorContactFieldElement* self = new (ELeave) CCreatorContactFieldElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop();
    return self;
    }

CCreatorContactFieldElement::CCreatorContactFieldElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    { }
