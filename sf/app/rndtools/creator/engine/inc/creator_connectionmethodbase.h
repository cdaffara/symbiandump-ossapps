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




#ifndef __CCREATORCONNECTIONSETTINGSBASE_H_
#define __CCREATORCONNECTIONSETTINGSBASE_H_

#include "creator_modulebase.h"

class CCreatorModuleBaseParameters;

const TUint KRandomBearerType = 0xEEEEEEEE;

class CCreatorConnectionSettingsBase : public CCreatorModuleBase
    {
public:
    
    virtual ~CCreatorConnectionSettingsBase(){};

    virtual TInt CreateConnectionSettingsEntryL(CCreatorModuleBaseParameters *aParameters) = 0;    
    virtual TUint32 AccessPointNameToIdL(const TDesC& aAPName, TBool aAnyIfNotFound = EFalse ) = 0;
    virtual void DeleteAllConnectionMethodsL(){};
    };

#endif /*__CCREATORCONNECTIONSETTINGSBASE_H_*/
