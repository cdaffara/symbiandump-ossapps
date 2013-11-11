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

#ifndef MEMSPYVIEWOBSERVER_H
#define MEMSPYVIEWOBSERVER_H

// User includes
#include "MemSpyViewType.h"

// Classes referenced
class CMemSpyViewBase;

class MMemSpyViewObserver
    {
public: // Enumerations
    enum TViewEventType
        {
        EEventItemActioned = 0,
        EEventItemSelected
        };

public: // From MMemSpyViewObserver
    virtual void HandleMemSpyViewEventL( TViewEventType aEvent, TMemSpyViewType aViewType, CMemSpyViewBase& aReportingView, TAny* aContext ) = 0;
    };


#endif
