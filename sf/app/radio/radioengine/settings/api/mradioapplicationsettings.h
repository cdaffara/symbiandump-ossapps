/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef M_RADIOAPPLICATIONSETTINGS_H
#define M_RADIOAPPLICATIONSETTINGS_H

#include <badesca.h>

/**
 * MRadioApplicationSettings
 *
 * Interface for managing persistent Visual Radio application settings.
 */
NONSHARABLE_CLASS( MRadioApplicationSettings )
    {

public:

    virtual TBool IsFirstTimeStart() = 0;

    virtual void SetFirstTimeStartPerformed( TBool aFirstTimeStartPerformed ) = 0;

    virtual TInt SetUiFlags( TUint aUiFlags ) = 0;

    virtual TUint UiFlags() const = 0;

    };

#endif // M_RADIOAPPLICATIONSETTINGS_H
