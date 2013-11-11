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

#ifndef C_RADIOAPPLICATIONSETTINGS_H
#define C_RADIOAPPLICATIONSETTINGS_H

// User includes
#include "mradioapplicationsettings.h"

// Forward declarations
class CRepository;

/**
 * Concrete implementation of application settings.
 */
NONSHARABLE_CLASS( CRadioApplicationSettings ) : public CBase
                                               , public MRadioApplicationSettings
    {

public:

    static CRadioApplicationSettings* NewL();

    ~CRadioApplicationSettings();

// from base class MRadioApplicationSettings

    TBool IsFirstTimeStart();
    void SetFirstTimeStartPerformed( TBool aFirstTimeStartPerformed );
    TInt SetUiFlags( TUint aUiFlags );
    TUint UiFlags() const;

private:

    CRadioApplicationSettings();

    void ConstructL();
    
private: // data
    CRepository* iRepository;

    };

#endif // C_RADIOAPPLICATIONSETTINGS_H
