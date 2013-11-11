// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimmobilitytesttls.h"
#include "cimmobilitytestframework.h"
#include "cimmobilitymanager.h"

/**
Factory constructor
Static routine

@return Constructed class
*/
CImMobilityTestTls* CImMobilityTestTls::NewL()
	{
	CImMobilityTestTls* self = static_cast<CImMobilityTestTls*>(Dll::Tls());

	if (!self)
		{
		self = new (ELeave) CImMobilityTestTls();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop(self);
		Dll::SetTls(self);
		}

	return self;
	}

/**
Class constructor
*/
CImMobilityTestTls::CImMobilityTestTls()
	{
	}

/**
Second phase constructor
*/
void CImMobilityTestTls::ConstructL()
	{
	}

/**
Class destructor
*/
CImMobilityTestTls::~CImMobilityTestTls()
	{
	iFrameworkList.Reset();

	Dll::SetTls(NULL);
	}

/**
Stores a test framework. Takes ownership of the framework.

@param aFramework Test framework
*/
void CImMobilityTestTls::AddFrameworkL(CImMobilityTestFramework& aFramework)
	{
	iFrameworkList.AppendL(&aFramework);
	}

/**
Removes a test framework from the list of frameworks and passes it back
to the caller. The caller takes ownership of the framework.

@param aMobilityManager Mobility manager associated with framework
@param aLastOne Last framework has now been removed
@return Test framework
*/
CImMobilityTestFramework* CImMobilityTestTls::RemoveFramework(CImMobilityManager& aMobilityManager, TBool& aLastOne)
	{
	CImMobilityTestFramework* framework = NULL;
	aLastOne = EFalse;

	for (TInt count = 0; count < iFrameworkList.Count(); ++count)
		{
		if (&(iFrameworkList[count]->MobilityManager()) == &aMobilityManager)
			{
			framework = iFrameworkList[count];
			iFrameworkList.Remove(count);
			if (iFrameworkList.Count() == 0)
				{
				aLastOne = ETrue;
				}
			break;
			}
		}

	return framework;
	}

/**
Gets a test framework from the list of frameworks.

@param aServiceId Service ID used as search key
@return Test framework
*/
CImMobilityTestFramework* CImMobilityTestTls::GetFramework(TMsvId aServiceId)
	{
	for (TInt count = 0; count < iFrameworkList.Count(); ++count)
		{
		if (iFrameworkList[count]->ServiceId() == aServiceId)
			{
			return iFrameworkList[count];
			}
		}

	return NULL;
	}

/**
Gets a test framework from the list of frameworks.

@param aMobilityManager Mobility manager used as search key
@return Test framework
*/
CImMobilityTestFramework* CImMobilityTestTls::GetFramework(CImMobilityManager& aMobilityManager)
	{
	for (TInt count = 0; count < iFrameworkList.Count(); ++count)
		{
		if (&(iFrameworkList[count]->MobilityManager()) == &aMobilityManager)
			{
			return iFrameworkList[count];
			}
		}

	return NULL;
	}

/**
Gets a test framework from the list of frameworks.

@param aProtocol MMobilityProtocolResp& used as search key
@return Test framework
*/
CImMobilityTestFramework* CImMobilityTestTls::GetFramework(MMobilityProtocolResp& aProtocol)
	{
	for (TInt count = 0; count < iFrameworkList.Count(); ++count)
		{
		CImMobilityManager* man = &(iFrameworkList[count]->MobilityManager());

		MMobilityProtocolResp* prot = static_cast<MMobilityProtocolResp*>(man);
		
		if (prot == &aProtocol)
			{
			return iFrameworkList[count];
			}
		}

	return NULL;
	}
