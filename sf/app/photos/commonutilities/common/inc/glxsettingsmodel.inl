/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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




//  CLASS HEADER
#include "glxsettingsmodel.h"

//  EXTERNAL INCLUDES
#include <centralrepository.h>
#include <featmgr.h>

//  INTERNAL INCLUDES
#include <glxlog.h>

namespace
    {
    const TUid KCRUidGallery = { 0x200009EE };
    }


// ----------------------------------------------------------------------------
// Retrieve value for a specific attribute uid
// ----------------------------------------------------------------------------
//
template <typename T>
inline T CGlxSettingsModel::ValueL(const TUint32& aSettingsId) 
    {
    GLX_LOG_INFO( "CGlxSettingsModel::ValueL" );
    CRepository* repository = CRepository::NewLC(KCRUidGallery);
    T retVal;
    User::LeaveIfError(repository->Get(aSettingsId, retVal));
    CleanupStack::PopAndDestroy(repository);
    repository = NULL;
    return retVal;
    }   
  
// ----------------------------------------------------------------------------
// Set the value for a specific attribute uid
// ----------------------------------------------------------------------------
//
template<typename T> 
inline void CGlxSettingsModel::SetValueL(const TUint32& aSettingsId, T aValue) 
	{
	GLX_LOG_INFO( "CGlxSettingsModel::SetValueL" );
/*
    TAny* ptr = User::AllocL(sizeof(T));
    memcpy(ptr, &aValue, sizeof(T));
*/
    CRepository* repository = CRepository::NewLC(KCRUidGallery);
    User::LeaveIfError(repository->Set(aSettingsId, aValue));
    CleanupStack::PopAndDestroy(repository);
    repository = NULL;
   	}