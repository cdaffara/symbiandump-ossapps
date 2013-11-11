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

#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmsubstates.hrh>

static TSsmState iState(ESsmNormal, ESsmNormalSubState);

RSsmStateAwareSession::RSsmStateAwareSession()
{

}
 
TInt RSsmStateAwareSession::Connect(TDmDomainId aId)
{
	TDmDomainId value = aId;
	return KErrNone;
}

TSsmState RSsmStateAwareSession::State() const
{
	return iState;
}

void RSsmStateAwareSession::AcknowledgeAndRequestStateNotification(TInt aError, TRequestStatus& aStatus)
{
    (void)aError;
    TRequestStatus* stat = &aStatus;
    User::RequestComplete( stat, KErrNone);
}

void RSsmStateAwareSession::AcknowledgeStateNotification(TInt aError)
{
    (void)aError;
}

void RSsmStateAwareSession::Close()
{

}	

void RSsmStateAwareSession::RequestStateNotification(TRequestStatus& aStatus)
{
    TRequestStatus* stat = &aStatus;
    User::RequestComplete( stat, KErrNone);
}

void RSsmStateAwareSession::RequestStateNotificationCancel()
{    
}

