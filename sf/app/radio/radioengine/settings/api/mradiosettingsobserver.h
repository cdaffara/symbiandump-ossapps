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

#ifndef M_RADIOSETTINGSOBSERVER_H
#define M_RADIOSETTINGSOBSERVER_H


/**
 * Interface for observing some radio related settings.
 * This interface is intended for the use of radio to update it's internal state
 * to conform to newly changed setting
 */
NONSHARABLE_CLASS( MRadioSettingsObserver )
    {
public:

    /**
     * Notifies of the change in RDS alternate frequency setting
     *
     * @param aEnabled ETrue if AF search is enabled, otherwise EFalse
     */
    virtual void RdsAfSearchSettingChangedL( TBool aEnabled ) = 0;

    /**
     * Notifies of the change in region setting when it is changed
     * outside of the particular radio settings instance
     *
     * @param aRegion The new region ID
     */
    virtual void RegionSettingChangedL( TInt aRegion ) = 0;

    };

#endif // M_RADIOSETTINGSOBSERVER_H
