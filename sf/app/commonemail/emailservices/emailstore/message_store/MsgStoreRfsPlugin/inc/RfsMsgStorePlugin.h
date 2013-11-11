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
* Description:  Implementation of RFS (Restore Factory Settings) Plugin i/f.
*
*/

#ifndef RFS_MSGSTOREPLUGIN_H_
#define RFS_MSGSTOREPLUGIN_H_

//  Include Files
#include <rfsPlugin.h>

// Forward declarations
class RFs;

NONSHARABLE_CLASS( CRfsMsgStorePlugin ) : public CRFSPlugin
    {
public:
    static CRfsMsgStorePlugin* NewL();
    ~CRfsMsgStorePlugin();

public:
    /**
     * Handle restory factory settings. Run at boot up.
     *
     * @param   aType type of boot
     * @return  none
     */
    void RestoreFactorySettingsL( const TRfsReason aType );

    /**
    * GetScriptL
    *
    * @param    aType type of boot
    * @param    aPath the path to the scripts where RFS server can execute
    * @return   None
    */
    void GetScriptL( const TRfsReason aType, TDes& aPath );

    /**
    * ExecuteCustomCommandL
    *
    * @param    aType type of boot
    * @param    aCommand custom command to be executed by this plugin
    * @return   None
    */
    void ExecuteCustomCommandL( const TRfsReason aType, TDesC& aCommand );

private:
    CRfsMsgStorePlugin();
    void ConstructL();

private: // Data
    RFs iFs;
    };

#endif // RFS_MSGSTOREPLUGIN_H_

