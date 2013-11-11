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
* Description:  For controlling S60 device lights.
*                Subscribes to a property to monitor changes in it.
*
*/


#ifndef HTIPROPERTYSUBSCRIBER_H
#define HTIPROPERTYSUBSCRIBER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* Subscribes to a property to monitor changes in it.
*/
class CHtiPropertySubscriber : public CActive
    {
    public:
        CHtiPropertySubscriber( TCallBack aCallBack, RProperty& aProperty );
        ~CHtiPropertySubscriber();

    public: // New functions
        void Subscribe();
        void Unsubscribe();

    private: // from CActive
        void RunL();
        void DoCancel();

    private: // data
        TCallBack   iCallBack;
        RProperty&  iProperty;
    };

#endif // HTIPROPERTYSUBSCRIBER_H

// End of file
