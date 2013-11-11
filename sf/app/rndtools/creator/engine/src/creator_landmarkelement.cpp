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



#include "creator_landmarkelement.h"
#include "creator_traces.h"
#include "creator_landmark.h"

using namespace creatorlandmark;

/*
 * 
 */
CCreatorLandmarkElement* CCreatorLandmarkElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorLandmarkElement* self = new (ELeave) CCreatorLandmarkElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorLandmarkElement::CCreatorLandmarkElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }

void CCreatorLandmarkElement::ExecuteCommandL()
    {
    const CCreatorScriptAttribute* amountAttr = FindAttributeByName(KAmount);
    TInt lmAmount = 1;    
    if( amountAttr )
        {
        lmAmount = ConvertStrToIntL(amountAttr->Value());
        }
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
        {
        // Get sub-elements
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();        
        // Create note entries, the amount of entries is defined by noteAmount:
        for( TInt cI = 0; cI < lmAmount; ++cI )
            {            
            CLandmarkParameters* param = new (ELeave) CLandmarkParameters;
            CleanupStack::PushL(param);
            
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();
                TPtrC elemContent = field->Content();
                const CCreatorScriptAttribute* randomAttr = fields[i]->FindAttributeByName(KRandomLength);
                TBool useRandom = EFalse;
                if( randomAttr || elemContent.Length() == 0 )
                    {
                    useRandom = ETrue;
                    }
                
                if( elemName == KName )
                    {
                    if( useRandom )
                        {
                        param->SetRandomNameL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iName, elemContent);
                        }
                    }
                else if( elemName == KCategory )
                    {
                    if( useRandom )
                        {
                        param->AddRandomCategoryL(*iEngine);
                        }                    
                    else
                        {
                        TPosLmItemId categoryId = CCreatorLandmarks::GetCategoryIdL(elemContent);
                        if( categoryId > 0 )
                            {
                            param->iCategories.AppendL(categoryId);
                            }
                        }
                    }
                else if( elemName == KDescription )
                    {
                    if( useRandom )
                        {
                        param->SetRandomDescriptionL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iDescription, elemContent);
                        }
                    }
                else if( elemName == KStreet )
                    {
                    if( useRandom )
                        {
                        param->SetRandomStreetL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iStreet, elemContent);
                        }
                    }
                else if( elemName == KPostalcode )
                    {
                    if( useRandom )
                        {
                        param->SetRandomPostCodeL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iPostCode, elemContent);
                        }
                    }
                else if( elemName == KCity )
                    {
                    if( useRandom )
                        {
                        param->SetRandomCityL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iCity, elemContent);
                        }
                    }
                else if( elemName == KState )
                    {
                    if( useRandom )
                        {
                        param->SetRandomStateL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iState, elemContent);
                        }
                    }
                else if( elemName == KCountry )
                    {
                    if( useRandom )
                        {
                        param->SetRandomCountryL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iCountry, elemContent);
                        }
                    }
                else if( elemName == KPhonenumber )
                    {
                    if( useRandom )
                        {
                        param->SetRandomPhoneNumberL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iPhonenumber, elemContent);
                        }
                    }
                else if( elemName == KUrl )
                    {
                    if( useRandom )
                        {
                        param->SetRandomUrlL(*iEngine);
                        }                    
                    else
                        {
                        SetContentToTextParamL(param->iUrl, elemContent);
                        }
                    }
                else if( elemName == KLatitude )
                    {
                    if( useRandom )
                        {
                        param->SetRandomLatitudeL(*iEngine);
                        }                    
                    else
                        {
                        ConvertStrToReal64L(elemContent, param->iLatitude);
                        }
                    }
                else if( elemName == KLongitude )
                    {
                    if( useRandom )
                        {
                        param->SetRandomLongitudeL(*iEngine);
                        }                    
                    else
                        {
                        ConvertStrToReal64L(elemContent, param->iLongitude);
                        }
                    }
                else if( elemName == KPositionaccuracy )
                    {
                    if( useRandom )
                        {
                        param->SetRandomPositionAccuracyL(*iEngine);
                        }                    
                    else
                        {
                        param->iPositionAccuracy = ConvertStrToIntL(elemContent);
                        }
                    }
                else if( elemName == KAltitude )
                    {
                    if( useRandom )
                        {
                        param->SetRandomAltitudeL(*iEngine);
                        }                    
                    else
                        {
                        ConvertStrToReal32L(elemContent, param->iAltitude);
                        }
                    }
                else if( elemName == KAltitudeaccuracy )
                    {
                    if( useRandom )
                        {
                        param->SetRandomAltitudeAccuracyL(*iEngine);
                        }                    
                    else
                        {
                        param->iAltitudeAccuracy = ConvertStrToIntL(elemContent);
                        }
                    }
                }
            iEngine->AppendToCommandArrayL(ECmdCreateMiscEntryLandmarks, param);
            CleanupStack::Pop(); // param
            }
        }
    else
    	{
    	iEngine->AppendToCommandArrayL(ECmdCreateMiscEntryLandmarks, 0, lmAmount);
    	}
    }
