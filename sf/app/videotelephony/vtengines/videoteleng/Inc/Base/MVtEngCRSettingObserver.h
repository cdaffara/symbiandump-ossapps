/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for indicating setting changes in Cent Rep
*
*/


#ifndef MVTENGSETTINCRGOBSERVER_H
#define MVTENGSETTINGCROBSERVER_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Observer interface for indicating setting changes
*
*  @lib videoteleng.lib
*  @since Series 60 3.0
*/
class MVtEngCRSettingObserver
    {
    public:  // New functions

        /**
        * Callback to notify changed setting
        * @param aUid changed Uid
        * @param aId changed id
        * @param aNewValue new value
        */
        virtual void HandleNotifyInt( const TUid aUid, const TUint32 aId, 
            TInt aNewValue ) = 0;

        /**
        * Callback to notify changed setting
        * @param aUid changed Uid
        * @param aId changed id
        * @param aNewValue new value
        */
        virtual void HandleNotifyReal( const TUid aUid, const TUint32 aId, 
            TReal aNewValue ) = 0;

        /**
        * Callback to notify changed setting
        * @param aUid changed Uid
        * @param aId changed id
        * @param aNewValue new value
        */
        virtual void HandleNotifyString( const TUid aUid, const TUint32 aId, 
            const TDesC16& aNewValue ) = 0;
    };

#endif      // MVTENGCRSETTINGOBSERVER_H   

// End of File
