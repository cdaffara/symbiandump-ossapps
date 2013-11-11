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
 
#include "irpropertychangeao.h"
#include "irpropertyobserver.h" 
#include "irqlogger.h" 

CIRPropertyChangeAO::CIRPropertyChangeAO(MIRPropertyChangeObserverInterface* aObserver, const TUid& aCategory,
        const TUint aKey) : CActive(CActive::EPriorityStandard), iObserver(aObserver), iCategory(aCategory), iKey(aKey)
{
    LOG_METHOD;
}

void CIRPropertyChangeAO::ConstructL()
{
    LOG_METHOD;
    User::LeaveIfError(iProperty.Attach(iCategory, iKey));
    CActiveScheduler::Add(this);     
}

CIRPropertyChangeAO* CIRPropertyChangeAO::NewL(
        MIRPropertyChangeObserverInterface* aObserver, const TUid& aCategory,
        const TUint aKey)
{
    LOG_METHOD;
    CIRPropertyChangeAO* self = new (ELeave) CIRPropertyChangeAO(aObserver,
            aCategory, aKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CIRPropertyChangeAO::~CIRPropertyChangeAO()
{
    Cancel();
    iProperty.Close();
}

void CIRPropertyChangeAO::ActivateL()
{
    if (!IsActive())
    {
        RunL();
    }
}

void CIRPropertyChangeAO::RunL()
{
    LOG_METHOD;
    
    if ( KErrNone == iStatus.Int() ) 
    {
        TInt err = KErrNone;

        err = iProperty.Get(iValueInt);
        
        if ( KErrNone == err )
        {            
            iObserver->HandlePropertyChangeL(iCategory, iKey, iValueInt);
        }    
        else
        {
            iObserver->HandlePropertyChangeErrorL(iCategory, iKey, err);
        }        
    }
    
    iProperty.Subscribe(iStatus);
    SetActive();
    
    //if the iStatus is error, ignore it.
}

void CIRPropertyChangeAO::DoCancel()
{
    iProperty.Cancel();
}

TBool CIRPropertyChangeAO::ValueInt(TInt& aValue)
{
	TInt tempValue = 0;
    TInt err = iProperty.Get(tempValue);
    
    if ( KErrNone == err )
    {
    	iValueInt = tempValue;    
    	aValue = tempValue;
    }
    else
    {
        return EFalse;
    }
    
    return ETrue;
}

