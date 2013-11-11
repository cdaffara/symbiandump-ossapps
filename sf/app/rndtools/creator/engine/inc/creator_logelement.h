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




#ifndef CREATORLOGELEMENT_H_
#define CREATORLOGELEMENT_H_

#include "creator_scriptelement.h"

namespace creatorlog
{
    _LIT(KLog, "log");
    _LIT(KDirection, "direction");
    _LIT(KDuration, "duration");
    _LIT(KPhonenumber, "phonenumber");
    _LIT(KDatetime, "datetime");
    _LIT(KMissed, "missed");
    _LIT(KIn, "in");
    _LIT(KOut, "out");
}

class CCreatorLogElement : public CCreatorScriptElement
{
public:
    static CCreatorLogElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
    
protected:
    CCreatorLogElement(CCreatorEngine* aEngine);
    TInt GetLogCommandL( const TDesC& aFileIdStr, TBool aRandom ) const;
};

#endif /*CREATORLOGELEMENT_H_*/
