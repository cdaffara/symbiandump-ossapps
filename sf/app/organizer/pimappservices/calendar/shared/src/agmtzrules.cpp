// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <vtzrules.h>
#include "agmtzrules.h"
#include "agmtlsproxy.h"

CAgnTzRules* CAgnTzRules::NewL()
	{
	CAgnTzRules* self = new (ELeave) CAgnTzRules();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAgnTzRules* CAgnTzRules::NewL(const CTzRules& aTzRules)
	{
	CAgnTzRules* self = new (ELeave) CAgnTzRules();
	CleanupStack::PushL(self);
	self->ConstructL(aTzRules);
	CleanupStack::Pop(self);
	return self;
	}

CAgnTzRules* CAgnTzRules::NewL(RReadStream& aStream, TBool aFromBuffer)
	{
	CAgnTzRules* self = new (ELeave) CAgnTzRules();
	CleanupStack::PushL(self);
	if(aFromBuffer)
		{
		self->ReadBufferL(aStream);
		}
	else
		{
		self->DoInternalizeL(aStream);
		}
	CleanupStack::Pop(self);
	return self;
	}

CAgnTzRules::~CAgnTzRules()
	{
	if(iTzRules && !SharedWithTzIndex())
		{
		CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
		tlsProxy->ReferenceRemoved(iTzRules);
		}
	}

CAgnTzRules::CAgnTzRules()
	: iTzRules(NULL), iTzZoneStreamId(KNullStreamId), iSharedInTzIndex(EFalse), iUseCurSysTzZone(EFalse)
	{
	}

void CAgnTzRules::ConstructL()
	{
	CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
	iTzRules = tlsProxy->GetCurrentSystemTzRulesL();
	SetUseCurrentSystemTzRules(ETrue);
	}

void CAgnTzRules::ConstructL(const CTzRules& aTzRules)
	{
	CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
	iTzRules = tlsProxy->GetZoneForRulesL(aTzRules);
	}

void CAgnTzRules::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iTzZoneIsSystemRule);
	aStream << iTzZoneStreamId;
	}

void CAgnTzRules::InternalizeL(RReadStream& aStream)
	{
	Reset();
	DoInternalizeL(aStream);
	}

void CAgnTzRules::DoInternalizeL(RReadStream& aStream)
	{
 	iTzZoneIsSystemRule = aStream.ReadUint8L();
  	aStream >> iTzZoneStreamId;
	}

void CAgnTzRules::Reset()
	{
	if(iTzRules && !SharedWithTzIndex())
		{
		CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
		tlsProxy->ReferenceRemoved(iTzRules);
		}
	
	iTzRules = NULL;
	SetTzZoneStreamId(KNullStreamId);
	SetSystemTzRule(EFalse);
	SetSharedWithTzIndex(EFalse);
	SetUseCurrentSystemTzRules(EFalse);
	}

//Externalize to buffer used by client to server requests.
void CAgnTzRules::WriteToBufferL(RWriteStream& aStream)
	{
	if(iTzRules == NULL || UseCurrentSystemTzRules())
		{
	   	aStream.WriteUint8L(ETrue);
	   	if(iTzRules == NULL)
	   		{
			CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
			iTzRules = tlsProxy->GetCurrentSystemTzRulesL();
			SetUseCurrentSystemTzRules(ETrue);
	   		}
		}
	else
		{
	   	aStream.WriteUint8L(EFalse);
	  	aStream << *iTzRules;;
		}
		
	aStream.WriteUint8L(iTzZoneIsSystemRule);
  	aStream << iTzZoneStreamId;
	}
	
//Internalize from buffer used by client to server requests.
void CAgnTzRules::ReadBufferL(RReadStream& aStream)
	{
	Reset();
	iUseCurSysTzZone = aStream.ReadUint8L();
  	
	CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
  	if(UseCurrentSystemTzRules())
  		{
		iTzRules = tlsProxy->GetCurrentSystemTzRulesL();
  		}
  	else
  		{
  		CTzRules* tzRules = CTzRules::NewL(aStream);
  		CleanupStack::PushL(tzRules);
  		iTzRules = tlsProxy->GetZoneForRulesL(*tzRules); // not owned
  		CleanupStack::PopAndDestroy(tzRules);
		}
  	
 	iTzZoneIsSystemRule = aStream.ReadUint8L();
	aStream >> iTzZoneStreamId;
	}
	
EXPORT_C const CTzRules* CAgnTzRules::TzRules() const
	{
	return iTzRules;
	}

EXPORT_C void CAgnTzRules::SetTzRules(CTzRules* aTzRules)
	{
	if(iTzRules && !SharedWithTzIndex())
		{
		CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
		tlsProxy->ReferenceRemoved(iTzRules);
		}
	
	iTzRules = aTzRules;
	SetSharedWithTzIndex(ETrue);
	}

CAgnTzRules* CAgnTzRules::CloneL() const
	{
	CAgnTzRules* tzZone = new (ELeave) CAgnTzRules();
	CleanupStack::PushL(tzZone);
	tzZone->CopyL(*this);
	CleanupStack::Pop(tzZone);
	return tzZone;
	}

CTzRules* CAgnTzRules::CloneTzRulesL() const
	{
	if(iTzRules)
		{
		return iTzRules->CloneL();
		}
	return NULL;
	}

void CAgnTzRules::CopyL(const CAgnTzRules& aAgnTzRules)
	{
	Reset();
	if(aAgnTzRules.iTzRules) 
		{
		if(!aAgnTzRules.SharedWithTzIndex())
			{
			CAgnTlsProxy* tlsProxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
			iTzRules = tlsProxy->GetZoneForRulesL(*aAgnTzRules.iTzRules);
			}
		else
			{
			iTzRules = aAgnTzRules.iTzRules;
			SetSharedWithTzIndex(ETrue);
			}
		}
	
	SetSystemTzRule(aAgnTzRules.SystemTzRule());
	SetUseCurrentSystemTzRules(aAgnTzRules.UseCurrentSystemTzRules());
	SetTzZoneStreamId(aAgnTzRules.TzZoneStreamId());
	}

EXPORT_C TStreamId CAgnTzRules::TzZoneStreamId() const
	{
	return (iTzZoneStreamId);
	}

EXPORT_C TBool CAgnTzRules::HasValidTzZoneStreamId() const
	{
	return (iTzZoneStreamId != KNullStreamId);
	}

EXPORT_C void CAgnTzRules::SetTzZoneStreamId(TStreamId aStreamId)
	{
	(iTzZoneStreamId = aStreamId);
	}

EXPORT_C TBool CAgnTzRules::SystemTzRule() const
	{
	return (iTzZoneIsSystemRule);
	}

EXPORT_C void CAgnTzRules::SetSystemTzRule(TBool aIsSystemRule)
	{
	(iTzZoneIsSystemRule = aIsSystemRule);
	}

TBool CAgnTzRules::SharedWithTzIndex() const
	{
	return (iSharedInTzIndex);
	}

void CAgnTzRules::SetSharedWithTzIndex(TBool aSharedWithTzIndex)
	{
	(iSharedInTzIndex = aSharedWithTzIndex);
	}

EXPORT_C TBool CAgnTzRules::UseCurrentSystemTzRules() const
	{
	return (iUseCurSysTzZone);
	}

EXPORT_C void CAgnTzRules::SetUseCurrentSystemTzRules(TBool aUseCurrentSystemTzRules)
	{
	(iUseCurSysTzZone = aUseCurrentSystemTzRules);
	}
