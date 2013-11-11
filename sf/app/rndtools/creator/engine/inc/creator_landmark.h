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




#ifndef LANDMARKS_H_
#define LANDMARKS_H_

#include "engine.h"
#include "creator_modulebase.h"

#include <e32base.h>
#include <EPos_Landmarks.h>

class CLandmarkParameters;
class CPosLandmarkDatabase;
class CPosLmOperation;

static const TInt KLandmarkFieldLength = 256;

class CCreatorLandmarks : public CCreatorModuleBase
    {
public: 
    static CCreatorLandmarks* NewL(CCreatorEngine* aEngine);
    static CCreatorLandmarks* NewLC(CCreatorEngine* aEngine);
    ~CCreatorLandmarks();

private:
    CCreatorLandmarks();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase    
    TPosLmItemId GetCategoryIdL( TPosLmGlobalCategory aGlobalCategoryID );

public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    
    TInt CreateLandmarkEntryL(CLandmarkParameters *aParameters);
    static TPosLmGlobalCategory GetCategoryIdL(const TDesC& aCategoryStr);
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();

private:
    CPosLandmarkDatabase* iLandmarkDb;    
    CLandmarkParameters* iParameters;
    CPosLmOperation* iLmOperation;
    RArray<TPosLmItemId> iEntryIds; // TPosLmItemId (==TUint32) CPosLandmark::LandmarkId()
public:
    };


class CLandmarkParameters : public CCreatorModuleBaseParameters
    {
public: 
    HBufC*              iName;
    RArray<TUint>       iCategories;
    HBufC*              iStreet;
    HBufC*              iCity;    
    HBufC*              iState;
    HBufC*              iCountry;
    HBufC*              iPostCode;    
    HBufC*              iDescription;
    HBufC*              iPhonenumber;
    HBufC*              iUrl;
    TReal64             iLatitude;
    TReal64             iLongitude;
    TReal32             iPositionAccuracy;
    TReal32             iAltitude;
    TReal32             iAltitudeAccuracy;
    
    void SetRandomNameL(CCreatorEngine& aEngine);
    void SetRandomUrlL(CCreatorEngine& aEngine);
    void AddRandomCategoryL(CCreatorEngine& aEngine);
    void SetRandomStreetL(CCreatorEngine& aEngine);
    void SetRandomCityL(CCreatorEngine& aEngine);
    void SetRandomStateL(CCreatorEngine& aEngine);
    void SetRandomCountryL(CCreatorEngine& aEngine);
    void SetRandomPostCodeL(CCreatorEngine& aEngine);
    void SetRandomLatitudeL(CCreatorEngine& aEngine);
    void SetRandomLongitudeL(CCreatorEngine& aEngine);
    void SetRandomPositionAccuracyL(CCreatorEngine& aEngine);
    void SetRandomAltitudeL(CCreatorEngine& aEngine);
    void SetRandomAltitudeAccuracyL(CCreatorEngine& aEngine);
    void SetRandomDescriptionL(CCreatorEngine& aEngine);
    void SetRandomPhoneNumberL(CCreatorEngine& aEngine);
    
private:
    
public:
    CLandmarkParameters();
    ~CLandmarkParameters();
    };
#endif /*LANDMARKS_H_*/
