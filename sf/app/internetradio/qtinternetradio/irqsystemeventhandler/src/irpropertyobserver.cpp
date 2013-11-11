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

//head files for symbian 
#include <e32def.h>  
#include <e32std.h>  
#include <eikdef.h>
#include <ctsydomainpskeys.h>

//private head files
#include "irsystemeventobserverinterface.h"
#include "irpropertyobserver.h" 
#include "irpropertychangeao.h"
#include "irqlogger.h"

 
CIRPropertyObserver::CIRPropertyObserver(MIRPropertyObserverInterface* aObserver) :
    iCallStatusObserver(NULL),iIsCallActive(EFalse),iPropertyObserver(aObserver)
{
    LOG_METHOD;
    Q_ASSERT(aObserver!=NULL);
}
 
void CIRPropertyObserver::ConstructL()
{    
    LOG_METHOD;
    
    TInt  callStatus = 0 ;
    TBool sucess = EFalse;
    
    iCallStatusObserver = CIRPropertyChangeAO::NewL(this, KPSUidCtsyCallInformation, KCTsyCallState);      
    iCallStatusObserver->ActivateL();
    
    sucess = iCallStatusObserver->ValueInt(callStatus);
    if( sucess )
    {
        iIsCallActive = ( callStatus > EPSCTsyCallStateNone );
    }
    else
    {
        iIsCallActive = EFalse;
    }     
}
 
CIRPropertyObserver::~CIRPropertyObserver()
{        
    delete iCallStatusObserver; 
    iCallStatusObserver = NULL;
}

 
CIRPropertyObserver* CIRPropertyObserver::NewL(MIRPropertyObserverInterface* aObserver)
{    
    LOG_METHOD;
    CIRPropertyObserver* self = NewLC(aObserver);  
    CleanupStack::Pop(self);    
    return self;
}

CIRPropertyObserver* CIRPropertyObserver::NewLC(MIRPropertyObserverInterface* aObserver)
{
    CIRPropertyObserver* self = new (ELeave) CIRPropertyObserver(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

TBool CIRPropertyObserver::IsCallActive() const
{
    return iIsCallActive;            
}

void CIRPropertyObserver::HandlePropertyChangeL(const TUid& aCategory,
        const TUint aKey, const TInt aValue)
{         
    LOG_FORMAT( "HandlePropertyChangeL_ENTRY. aKey = %d, aValue = %d", aKey, aValue);
    if (aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState)
    {
        if ((!iIsCallActive) && (aValue > EPSCTsyCallStateNone))
        { 
            iIsCallActive = ETrue;
            iPropertyObserver->callIsActivated();
        }
        else if ((iIsCallActive) && (aValue <= EPSCTsyCallStateNone))
        {
            iIsCallActive = EFalse;
            iPropertyObserver->callIsDeactivated();
        }
    }
}

void CIRPropertyObserver::HandlePropertyChangeErrorL(const TUid& aCategory,
        const TUint aKey, const TInt aError)
{
    LOG_METHOD;
    Q_UNUSED(aCategory);
    Q_UNUSED(aKey);
    iPropertyObserver->errorCallback(aError);
}
