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




#ifndef TCREATORFACTORY_H_
#define TCREATORFACTORY_H_

#include "creator_modulebase.h"

class CCreatorEngine;
class CCreatorPhonebookBase;
//class CCreatorConnectionSettingsBase;


class TCreatorFactory{

public: 
	static CCreatorPhonebookBase* CreatePhoneBookL(CCreatorEngine* aEngine);
	static CCreatorModuleBaseParameters* CreatePhoneBookParametersL();

//    static CCreatorConnectionSettingsBase* CreateConnectionSettingsL(CCreatorEngine* aEngine);
//    static CCreatorModuleBaseParameters* CreateConnectionSettingsParametersL();
	

};

#endif /*TCREATORFACTORY_H_*/
