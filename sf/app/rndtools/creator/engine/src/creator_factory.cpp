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


#include "engine.h"
#include "creator_factory.h"

//#include "creator_accesspoint.h"
#include "creator_phonebook.h"

CCreatorPhonebookBase* TCreatorFactory::CreatePhoneBookL(CCreatorEngine* aEngine)
{
    return CCreatorPhonebook::NewL(aEngine);    
}

CCreatorModuleBaseParameters* TCreatorFactory::CreatePhoneBookParametersL()
	{
	return new(ELeave) CPhonebookParameters;
	}
/*
CCreatorConnectionSettingsBase* TCreatorFactory::CreateConnectionSettingsL(CCreatorEngine* aEngine)
{
    return CCreatorAccessPoints::NewL(aEngine); 
}

CCreatorModuleBaseParameters* TCreatorFactory::CreateConnectionSettingsParametersL()
    {
    return new(ELeave) CAccessPointsParameters;
    }
*/
