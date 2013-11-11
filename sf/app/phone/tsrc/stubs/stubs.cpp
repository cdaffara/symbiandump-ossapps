/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
    
#include "cphonemediatorfactory.h"

CPhoneMediatorFactory* CPhoneMediatorFactory::Instance()
    {
    
    }
    
CPhoneMediatorSender* CPhoneMediatorFactory::Sender()
    {

    }

#include "cphonemediatorsender.h"
    
TInt CPhoneMediatorSender::IssueCommand( 
    const TUid /*aDomain*/, 
    const TUid /*aCategory*/, 
    const TInt /*aCommandId*/,
    const TVersion /*aVersion*/, 
    const TDesC8& /*aData*/,
    MPhoneShutdownCommand* /*aShutdownCommand*/ )
    {
    
    }
    
void CPhoneMediatorSender::SendEvent( const TPhoneViewCommandId aCommandId, 
    const TInt aCallId, TPhoneCommandParam& aCommandParam ) const
    {
    
    }
