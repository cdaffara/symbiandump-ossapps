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

#ifndef IRPROPERTYOBSERVER_H
#define IRPROPERTYOBSERVER_H 

#include <e32base.h>

//This is an symbian interface.  
class MIRPropertyChangeObserverInterface
{

public:

    virtual void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TInt aValue) = 0;
    virtual void HandlePropertyChangeErrorL(const TUid& aCategory, const TUint aKey, TInt aError) = 0;
};

class MIRPropertyObserverInterface;
class CIRPropertyChangeAO;

class CIRPropertyObserver : public CBase, public MIRPropertyChangeObserverInterface
{
public:

    //only two-phase constructor is permit to use
    static  CIRPropertyObserver* NewL(MIRPropertyObserverInterface* aObserver);
    static  CIRPropertyObserver* NewLC(MIRPropertyObserverInterface* aObserver);
    ~CIRPropertyObserver();
    TBool    IsCallActive() const;

protected:

    //from base class MIRPropertyChangeObserverInterface
    void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TInt aValue);
    void HandlePropertyChangeErrorL(const TUid& aCategory, const TUint aKey, TInt aError);

private:

    void ConstructL();
    CIRPropertyObserver(MIRPropertyObserverInterface* aObserver);

private: 
    
    CIRPropertyChangeAO* iCallStatusObserver;
    TBool                iIsCallActive;
    
    MIRPropertyObserverInterface* iPropertyObserver;
};
 
#endif	//IRPROPERTYOBSERVER_H

