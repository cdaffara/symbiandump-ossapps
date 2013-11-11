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


#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmCategoryManager.h>
#include <lbsposition.h>

#include "engine.h"
#include "enginewrapper.h"
#include "creator_landmark.h" 
#include "creator_traces.h"

_LIT(KAccommodation, "accommodation");
_LIT(KBusiness, "business");
_LIT(KCommunication, "communication");
_LIT(KEducational, "educational");
_LIT(KEntertainment, "entertainment");
_LIT(KFoodAndBeverage, "food");
_LIT(KGeographical, "geographical");
_LIT(KOutdoor, "outdoor");
_LIT(KPeople, "people");
_LIT(KPublic, "public");
_LIT(KReligious, "religious");
_LIT(KShopping, "shopping");
_LIT(KSightseeing, "sightseeing");
_LIT(KSports, "sports");
_LIT(KTransport, "transport");


/**
 * Class mapping the strings to global landmark categories
 */
class CategoryMapping
{
public:
    CategoryMapping(TUint aId, const TDesC& aCategoryStr)
    : 
    iGlobalId(aId),
    iStr(aCategoryStr)
    {}

    TPosLmGlobalCategory iGlobalId;
    const TDesC& iStr;
};

/**
 * Global landmark categories
 */
CategoryMapping LandmarkCategories[] =    {
        CategoryMapping(3000, KAccommodation), // Accommodation
        CategoryMapping(6000, KBusiness), // Business
        CategoryMapping(9000, KCommunication), // Communication
        CategoryMapping(12000, KEducational), // Educational institute
        CategoryMapping(15000, KEntertainment), // Entertainment
        CategoryMapping(18000, KFoodAndBeverage), // Food & Beverage
        CategoryMapping(21000, KGeographical), // Geographical area
        CategoryMapping(24000, KOutdoor), // Outdoor activities        
        CategoryMapping(27000, KPeople), // People
        CategoryMapping(30000, KPublic), // Public service
        CategoryMapping(33000, KReligious), // Religious places
        CategoryMapping(36000, KShopping), // Shopping
        CategoryMapping(39000, KSightseeing), // Sightseeing
        CategoryMapping(42000, KSports), // Sports
        CategoryMapping(45000, KTransport)  // Transport
};

/**
 * Return landmark global id
 */ 
TPosLmGlobalCategory CCreatorLandmarks::GetCategoryIdL(const TDesC& aCategoryStr)
    {
    TUint numOfCategories = sizeof(LandmarkCategories)/sizeof(CategoryMapping);
    for( TUint i = 0; i < numOfCategories; ++i )
        {
        if( LandmarkCategories[i].iStr == aCategoryStr )
            return LandmarkCategories[i].iGlobalId;
        }
    
    return 0;
    }

/**
 *
 */ 
CLandmarkParameters::CLandmarkParameters()
    {
    LOGSTRING("Creator: CLandmarkParameters::CLandmarkParameters");
    }

/**
 *
 */
CLandmarkParameters::~CLandmarkParameters()
    {
    LOGSTRING("Creator: CLandmarkParameters::~CLandmarkParameters");
    delete iName;    
    delete iStreet;
    delete iCity;    
    delete iState;
    delete iCountry;
    delete iPostCode;    
    delete iDescription;
    delete iPhonenumber;
    delete iUrl;
    iCategories.Close();
    }

/**
 *
 */
void CLandmarkParameters::SetRandomNameL(CCreatorEngine& aEngine)
    {    
    TPtrC name(aEngine.RandomString(CCreatorEngine::ELandmarkName));
    delete iName;
    iName = 0;
    iName = HBufC::NewL(name.Length());
    iName->Des().Copy(name);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomUrlL(CCreatorEngine& aEngine)
    {
    _LIT(KUrlPrefix, "http://www.");
    _LIT(KUrlPostfix, ".com");
    TPtrC company(aEngine.RandomString(CCreatorEngine::ECompany));
    delete iUrl;
    iUrl = 0;
    iUrl = HBufC::NewL(KUrlPrefix().Length()+company.Length()+KUrlPostfix().Length());
    iUrl->Des() = KUrlPrefix;
    iUrl->Des().Append( company );
    iUrl->Des().Append( KUrlPostfix );
    }

/**
 *
 */
void CLandmarkParameters::AddRandomCategoryL(CCreatorEngine& aEngine)
    {
    TUint numOfCategories = sizeof(LandmarkCategories)/sizeof(CategoryMapping);
    iCategories.AppendL(LandmarkCategories[aEngine.RandomNumber(0, numOfCategories-1)].iGlobalId);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomStreetL(CCreatorEngine& aEngine)
    {
    TPtrC street(aEngine.RandomString(CCreatorEngine::EAddress));
    delete iStreet;
    iStreet = 0;
    iStreet = HBufC::NewL(street.Length());
    iStreet->Des().Copy(street);
    }
 
/**
 *
 */
void CLandmarkParameters::SetRandomCityL(CCreatorEngine& aEngine)
    {
    TPtrC city(aEngine.RandomString(CCreatorEngine::ECity));
    delete iCity;
    iCity = 0;
    iCity = HBufC::NewL(city.Length());
    iCity->Des().Copy(city);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomStateL(CCreatorEngine& aEngine)
    {
    TPtrC state(aEngine.RandomString(CCreatorEngine::EState));
    delete iState;
    iState = 0;
    iState = HBufC::NewL(state.Length());
    iState->Des().Copy(state);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomCountryL(CCreatorEngine& aEngine)
    {
    TPtrC country(aEngine.RandomString(CCreatorEngine::ECountry));
    delete iCountry;
    iCountry = 0;
    iCountry = HBufC::NewL(country.Length());
    iCountry->Des().Copy(country);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomPostCodeL(CCreatorEngine& aEngine)
    {
    TPtrC po(aEngine.RandomString(CCreatorEngine::EPostcode));
    delete iPostCode;
    iPostCode = 0;
    iPostCode = HBufC::NewL(po.Length());
    iPostCode->Des().Copy(po);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomLatitudeL(CCreatorEngine& aEngine)
    {
    iLatitude = aEngine.RandomNumber(-89, 89); // Degrees
    TReal64 random = aEngine.RandomNumber(0, 9999); // Desimals 
    iLatitude +=  random / 10000.0;
    }

/**
 *
 */
void CLandmarkParameters::SetRandomLongitudeL(CCreatorEngine& aEngine)
    {
    TInt minDegrees = -179;
    TInt maxDegrees = 179;
    TInt minDecimal = 0;
    TInt maxDecimal = 9999;
    TReal64 divider = 10000.0;
    iLongitude = aEngine.RandomNumber(minDegrees, maxDegrees); // Degrees
    TReal64 random = aEngine.RandomNumber(minDecimal, maxDecimal); 
    iLongitude += random  / divider;
    }

/**
 *
 */
void CLandmarkParameters::SetRandomPositionAccuracyL(CCreatorEngine& aEngine)
    {
    this->iPositionAccuracy = aEngine.RandomNumber(0,5);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomAltitudeL(CCreatorEngine& aEngine)
    {
    this->iAltitude = aEngine.RandomNumber(0,120);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomAltitudeAccuracyL(CCreatorEngine& aEngine)
    {
    this->iAltitudeAccuracy = aEngine.RandomNumber(0,5);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomDescriptionL(CCreatorEngine& aEngine)
    {
    delete iDescription;
    iDescription = 0;
    TPtrC desc(aEngine.RandomString(CCreatorEngine::ELandmarkDescription));
    iDescription = HBufC::NewL(desc.Length());
    iDescription->Des().Copy(desc);
    }

/**
 *
 */
void CLandmarkParameters::SetRandomPhoneNumberL(CCreatorEngine& aEngine)
    {
    delete iPhonenumber;
    iPhonenumber = 0;
    TPtrC phone(aEngine.RandomString(CCreatorEngine::EPhoneNumber));
    iPhonenumber = HBufC::NewL(phone.Length());
    iPhonenumber->Des().Copy(phone);
    }

/**
 *
 */
CCreatorLandmarks::CCreatorLandmarks()
    {
    }

/**
 *
 */
CCreatorLandmarks* CCreatorLandmarks::NewL(CCreatorEngine* aEngine)
    {
    CCreatorLandmarks* self = CCreatorLandmarks::NewLC(aEngine);
    CleanupStack::Pop(); // self
    return self;
    }

/**
 *
 */
CCreatorLandmarks* CCreatorLandmarks::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorLandmarks* self = new (ELeave) CCreatorLandmarks;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

/**
 *
 */
void CCreatorLandmarks::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorLandmarks::ConstructL");
    iEngine = aEngine;
    iLandmarkDb = CPosLandmarkDatabase::OpenL();
    if( iLandmarkDb->IsInitializingNeeded() )
        {
        ExecuteAndDeleteLD(iLandmarkDb->InitializeL());
        }
    }

/**
 *
 */
CCreatorLandmarks::~CCreatorLandmarks()
    {
    LOGSTRING("Creator: CCreatorLandmarks::~CCreatorLandmarks");
    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidLandmarks ) );
        }
    iEntryIds.Reset();
    iEntryIds.Close();
    
    delete iLandmarkDb;
    delete iLmOperation;
    delete iParameters;
    }

/**
 *
 */
TBool CCreatorLandmarks::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorLandmarks::AskDataFromUserL");
    
    CCreatorModuleBase::AskDataFromUserL( aCommand );
    
    if ( aCommand == ECmdDeleteLandmarks )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Landmarks?"), this, ECreatorModuleDelete );
        }
    else if ( aCommand ==  ECmdDeleteCreatorLandmarks )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all Landmarks created with Creator?"), this, ECreatorModuleDelete );
        }
    return iEngine->GetEngineWrapper()->EntriesQueryDialog(&iEntriesToBeCreated, _L("How many landmarks to create?"), EFalse, this, ECreatorModuleStart );    
    }

/**
 * Convert global category id to local category id
 */
TPosLmItemId CCreatorLandmarks::GetCategoryIdL( TPosLmGlobalCategory aGlobalCategoryID )
{
    CPosLmCategoryManager* categoryManager = CPosLmCategoryManager::NewL( *iLandmarkDb );
    CleanupStack::PushL( categoryManager );

    // Find ID of the given global category in given database
    TPosLmItemId id = categoryManager->GetGlobalCategoryL( aGlobalCategoryID );
    CleanupStack::PopAndDestroy(); // categoryManager
    return id;
}

/**
 *
 */
TInt CCreatorLandmarks::CreateLandmarkEntryL(CLandmarkParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorLandmarks::CreateLandmarkEntryL");
    TInt err = KErrNone;
    // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
        
    CLandmarkParameters* parameters = aParameters;
        
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CLandmarkParameters;
        parameters = iParameters;
            
        TBuf<160> company = iEngine->RandomString(CCreatorEngine::ECompany);

        parameters->SetRandomNameL(*iEngine);
        parameters->SetRandomUrlL(*iEngine);
        parameters->AddRandomCategoryL(*iEngine);
        parameters->SetRandomStreetL(*iEngine);
        parameters->SetRandomCityL(*iEngine);
        parameters->SetRandomStateL(*iEngine);
        parameters->SetRandomCountryL(*iEngine);
        parameters->SetRandomPostCodeL(*iEngine);
        parameters->SetRandomLatitudeL(*iEngine);
        parameters->SetRandomLongitudeL(*iEngine);
        parameters->SetRandomPositionAccuracyL(*iEngine);
        parameters->SetRandomAltitudeL(*iEngine);
        parameters->SetRandomAltitudeAccuracyL(*iEngine);
        parameters->SetRandomDescriptionL(*iEngine);
        parameters->SetRandomPhoneNumberL(*iEngine);
        }
    
    CPosLandmark* newLandmark = CPosLandmark::NewLC();        
    TCoordinate crd(parameters->iLatitude, parameters->iLongitude, parameters->iAltitude);
    TLocality loc(crd, parameters->iPositionAccuracy, parameters->iAltitudeAccuracy);
    newLandmark->SetPositionL(loc);
    
    // Add categories
    for( TInt i = 0; i < parameters->iCategories.Count(); ++i )
        {
        newLandmark->AddCategoryL(GetCategoryIdL(parameters->iCategories[i]));
        }
    if( parameters->iName )    
        newLandmark->SetLandmarkNameL(parameters->iName->Des());    
    if( parameters->iCountry )
        newLandmark->SetPositionFieldL(EPositionFieldCountry, parameters->iCountry->Des());
    if( parameters->iCity )
        newLandmark->SetPositionFieldL(EPositionFieldCity, parameters->iCity->Des());
    if( parameters->iPostCode )
        newLandmark->SetPositionFieldL(EPositionFieldPostalCode, parameters->iPostCode->Des());
    if( parameters->iState )
        newLandmark->SetPositionFieldL(EPositionFieldState, parameters->iState->Des());
    if( parameters->iStreet )
        newLandmark->SetPositionFieldL(EPositionFieldStreet, parameters->iStreet->Des());
    if( parameters->iPhonenumber )
        newLandmark->SetPositionFieldL(EPositionFieldBuildingTelephone, parameters->iPhonenumber->Des());
    if( parameters->iDescription )
        newLandmark->SetLandmarkDescriptionL(parameters->iDescription->Des());    
    if( parameters->iUrl )
        newLandmark->SetPositionFieldL(EPositionFieldMediaLinksStart, parameters->iUrl->Des());
    
    TRAP(err, iLandmarkDb->AddLandmarkL(*newLandmark));
    
    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( newLandmark->LandmarkId() );

    ExecuteAndDeleteLD(iLandmarkDb->CompactL());
    
    CleanupStack::PopAndDestroy(newLandmark);
    return err;
    }

//----------------------------------------------------------------------------
void CCreatorLandmarks::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorLandmarks::DeleteAllL");
    ExecuteAndDeleteLD( iLandmarkDb->RemoveAllLandmarksL() );
    ExecuteAndDeleteLD( iLandmarkDb->CompactL() );

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Landmarks related registry
    iEngine->RemoveStoreL( KUidDictionaryUidLandmarks );
    }

//----------------------------------------------------------------------------
void CCreatorLandmarks::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorLandmarks::DeleteAllCreatedByCreatorL");
    iEntryIds.Reset();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidLandmarks );
    
    // delete entries
    TRAP_IGNORE(
        ExecuteAndDeleteLD( iLandmarkDb->RemoveLandmarksL( iEntryIds ) );
        ExecuteAndDeleteLD( iLandmarkDb->CompactL() );
        );

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Landmarks related registry
    iEngine->RemoveStoreL( KUidDictionaryUidLandmarks );
    }


