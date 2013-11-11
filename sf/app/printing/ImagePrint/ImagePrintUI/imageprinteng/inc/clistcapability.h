/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CLISTCAPABILITY_H
#define CLISTCAPABILITY_H

#include "cbasecapability.h"

class CRealFactory;

/**
 *
 *  Capability type for list type of settings
 *
 */
class CListCapability :
    public CBaseCapability
    {
    public:     //  Constructors and destructors
        
        CListCapability ( CRealFactory* aFactory );
        
        /**
         *  Destructor
         */
        virtual ~CListCapability();

    public:     //  Methods derived from CBaseCapability

        virtual HBufC* ListBoxTextL();
        virtual TBool LaunchPopupListL();
        virtual CBaseCapability* CloneL();

    public:     // Data

        // Enumeration IDs and the texts associated to the ids
        RArray<TInt> iEnumIDs;
        RPointerArray<HBufC> iTexts;      
        
        CRealFactory* iFactory; //not owned  
    };

#endif  //  CLISTCAPABILITY_H

//  End of File
