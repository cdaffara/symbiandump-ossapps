// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecom/ecom.h>

const TUid KUidImMobilityPolicy = {0x10283066};

/**
CleanUpEComSmsInfoArray function for cleanup support of locally declared arrays.
*/
void CleanUpEComInfoArray(TAny* aInfoArray)
	{
	RImplInfoPtrArray* infoArray = (static_cast<RImplInfoPtrArray*>(aInfoArray));
	infoArray->ResetAndDestroy();
	infoArray->Close();
	}
	
/**
Static factory constructor.

@return The constructed CImMobilityPolicyPlugin object
*/
inline CImMobilityPolicyPlugin* CImMobilityPolicyPlugin::NewL()
	{
	RImplInfoPtrArray implInfoArray;
	TCleanupItem cleanup(CleanUpEComInfoArray, &implInfoArray);
    CleanupStack::PushL(cleanup);

    // get all implementations of the mobility policy plugin interface
	REComSession::ListImplementationsL(KUidImMobilityPolicy, implInfoArray);
	TInt count = implInfoArray.Count();

	if (count == 0)
		{
		implInfoArray.ResetAndDestroy();
		implInfoArray.Close();
		CleanupStack::PopAndDestroy(&implInfoArray);
		User::Leave(KErrNotFound);
		}
		
	CImMobilityPolicyPlugin* interface = NULL;

	for (TInt impl = 0; impl < count; ++impl)
		{
		if (implInfoArray[impl]->VendorId() != 0x70000001)
			{
			interface = REINTERPRET_CAST(
							CImMobilityPolicyPlugin*,
							REComSession::CreateImplementationL(
								implInfoArray[impl]->ImplementationUid(),
								_FOFF(CImMobilityPolicyPlugin, iDtor_ID_Key)));
			}
		}
	
	if (!interface)
		{
		interface = REINTERPRET_CAST(
						CImMobilityPolicyPlugin*,
						REComSession::CreateImplementationL(
							implInfoArray[0]->ImplementationUid(),
							_FOFF(CImMobilityPolicyPlugin, iDtor_ID_Key)));
		}
				
	implInfoArray.ResetAndDestroy();
	implInfoArray.Close();
    CleanupStack::PopAndDestroy(&implInfoArray);
	return interface;
	}

/**
Class constructor.
*/
inline CImMobilityPolicyPlugin::CImMobilityPolicyPlugin()
	{
	}

/**
Class destructor.
Destroy the ECOM implementation
*/
inline CImMobilityPolicyPlugin::~CImMobilityPolicyPlugin()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}




	
