/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mpxcollectionhelperfactory stub for testing mpmpxframeworkwrapper 
*
*/



#ifndef MMPXCOLLECTIONHELPERFACTORY_H
#define MMPXCOLLECTIONHELPERFACTORY_H

#include <bautils.h>
#include <mpxcollectionframeworkdefs.h>

// FORWARD DECLARATIONS
class MMPXCollectionUiHelper;
 
/**
 *  Factory stub class to construct collection helpers.
 */
class CMPXCollectionHelperFactory : public CBase
{

public:
    
    // Stub functions
    static MMPXCollectionUiHelper* NewCollectionUiHelperL(const TUid& aModeId = KMcModeDefault);

    
};
       
#endif // MMPXCOLLECTIONHELPERFACTORY_H
