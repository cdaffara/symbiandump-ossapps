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




#ifndef CREATORLANDMARKELEMENT_H_
#define CREATORLANDMARKELEMENT_H_

#include "creator_scriptelement.h"

namespace creatorlandmark
{
    _LIT(KLandmark, "landmark");
    _LIT(KName, "name");
    _LIT(KCategory, "category");
    _LIT(KDescription, "description");
    _LIT(KStreet, "street");
    _LIT(KPostalcode, "postalcode");
    _LIT(KCity, "city");
    _LIT(KState, "state");
    _LIT(KCountry, "country");
    _LIT(KPhonenumber, "phonenbr");
    _LIT(KUrl, "url");
    _LIT(KLatitude, "latitude");
    _LIT(KLongitude, "longitude");
    _LIT(KPositionaccuracy, "positionaccuracy");
    _LIT(KAltitude, "altitude");
    _LIT(KAltitudeaccuracy, "altitudeaccuracy");
}

class CCreatorLandmarkElement : public CCreatorScriptElement
{
public:
    static CCreatorLandmarkElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    
protected:
    CCreatorLandmarkElement(CCreatorEngine* aEngine);
};

#endif /*CREATORLANDMARKELEMENT_H_*/
