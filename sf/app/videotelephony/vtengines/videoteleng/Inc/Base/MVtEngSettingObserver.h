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
* Description:  Observer interface for indicating setting changes
*
*/



#ifndef MVTENGSETTINGOBSERVER_H
#define MVTENGSETTINGOBSERVER_H

//  INCLUDES
#include "CVtEngSettings.h"

// CLASS DECLARATION

/**
*  Observer interface for indicating setting changes
*
*  @lib videoteleng
*  @since 2.6
*/
class MVtEngSettingObserver
    {
    public:  // New functions

        /**
        * Handler for setting changed event
        * @param aId id of setting
        * @param aValue new value for the setting
        */
        virtual void HandleSettingChangedL( 
            CVtEngSettings::TSettingId aId, 
            const TDesC& aValue ) = 0;
    };

#endif      // MVTENGSETTINGOBSERVER_H   

// End of File
