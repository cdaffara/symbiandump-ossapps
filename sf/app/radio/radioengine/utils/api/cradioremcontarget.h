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

#ifndef CRADIOREMCONTARGET_H
#define CRADIOREMCONTARGET_H

// System includes
#include <e32base.h>

// Forward declarations
class MRadioControlEventObserver;

// Class declaration

NONSHARABLE_CLASS( CRadioRemConTarget ) : public CBase
    {
public:

    IMPORT_C static CRadioRemConTarget* NewL();

    virtual ~CRadioRemConTarget();

    /**
     * Sets control event observer.
     * @param aControlEventObserver the observer for control events
     */
    virtual void SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver ) = 0;

    };

#endif // CRADIOREMCONTARGET_H
