/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef CIRPROPERTYCHANGEAO_H
#define CIRPROPERTYCHANGEAO_H

#include <e32base.h>
#include <e32property.h>

class MIRPropertyChangeObserverInterface;

class CIRPropertyChangeAO : public CActive
{
    
public:
    
    //only two-phase constructor is permit to use
    static CIRPropertyChangeAO* NewL( MIRPropertyChangeObserverInterface* aObserver, const TUid& aCategory, const TUint aKey);
    ~CIRPropertyChangeAO();
    void ActivateL();
    TBool ValueInt(TInt& aValue);
    
private:
 
    CIRPropertyChangeAO(MIRPropertyChangeObserverInterface* aObserver,const TUid& aCategory,const TUint aKey);
    void ConstructL();

protected:

    void RunL();
    void DoCancel();

private:

    TInt            iValueInt;
    RProperty       iProperty;
    MIRPropertyChangeObserverInterface* iObserver;
    TUid            iCategory;
    TUint           iKey;
};
#endif  // CIRPROPERTYCHANGEAO_H

