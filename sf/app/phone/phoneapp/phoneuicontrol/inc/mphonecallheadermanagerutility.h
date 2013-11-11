/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for setting and getting CLI related from engine.
*
*/

#ifndef MPHONECALLHEADERMANAGERUTILITY_H
#define MPHONECALLHEADERMANAGERUTILITY_H

#include "tphonecmdparamcallheaderdata.h"
#include "mphonecustomization.h"

/**
* Utily Interface for loading resources and using customization.
*
* @since S60 v9.1
*/
class MPhoneCallHeaderManagerUtility
    {
    public:

        /**
        * Loads strings for displaying in call header.
        * @param aCallLabelId - Resource Id for longer text.
        * @param aShortLabelId - Resource id for short text
        * @param aCallHeaderData - Call header parameter into which the text
        *                          will be set.
        */
        virtual void LoadCallHeaderTexts(
            const TInt aLabelId, 
            const TInt aShortLabelId,
            TPhoneCmdParamCallHeaderData* aCallHeaderData ) = 0; 
        
        /**
        * Loads data commonengine stringloader.                           
        * @param aData - The returned data including needed text.
        * @param aResourceId - Resource id for text.
        */
        virtual void LoadResource( TDes& aData, const TInt aResourceId ) = 0;
        
        /**
        * Returns instance of class MPhoneCustomization
        */
        virtual MPhoneCustomization* Customization() = 0;
        
    };

#endif // MPHONECALLHEADERMANAGERUTILITY_H
