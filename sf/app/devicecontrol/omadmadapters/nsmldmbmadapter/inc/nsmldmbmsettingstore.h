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
* Description:   The class manages snap data in Commsdat using CMManager
*
*/




#ifndef NSMLDMBMSETTINGSTORE_H
#define NSMLDMBMSETTINGSTORE_H

#include <cmmanagerext.h>

#include "nsmldmbmadapter.h"

const TInt KShift8 = 8;

class CNSmlDmBmAdapter;
class CSmlDmBmCommandElement;
class CSmlDmBmSettingsElement;

/**
 *  Setting Store is used for managing settings in CommsDat
 *
 *  The class provides functions for executing commands 
 *  and sets of commands that involve manipulation of 
 *  data in CommsDat. 
 *
 *  @lib nsmldmbmadapter
 *  @since S60 v3.2
 */
class CNSmlDmBmSettingStore : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CNSmlDmBmSettingStore * NewL( CNSmlDmBmAdapter* aAdapter );

    /**
     * Destructor.
     */
    virtual ~CNSmlDmBmSettingStore();


    /**
     * Executes all commands for the argument snap object.
     * Executed commands are set as executed and status is filled 
     * in. For Get commands also fetched data is filled in the command.
     *
     * @since S60 v3.2
     * @param aSettings Settings object for the SNAP, contains all 
     * commands to be executed for the SNAP
     */
    void StoreSnapL( CSmlDmBmSettingsElement& aSettings );

    
    /**
     * Executes the argument command. Checks the node from 
     * for which the command is intended and calls node specific functions.
     *
     * @since S60 v3.2
     * @param aCmd Command to be executed
     * @param aLuid LUID of the SNAP which command is referring.
     */
     void ExecuteCmdL( CSmlDmBmCommandElement& aCmd, TUint aLuid);

    /**
     * Fetches all SNAPs and inserts id of each SNAp into the argument array
     *
     * @since S60 v3.2
     * @param aLUIDArray Array for SNAP ids
     */
    void GetSnapsL( RArray<TUint32>& aLUIDArray );
 
private:

    CNSmlDmBmSettingStore( CNSmlDmBmAdapter * aAdapter );

    void ConstructL();

//----------- Commands for managing leaf nodes -------------------//

    /**
     * Executes snap commands. The valid commands include 
     * get and delete. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aLuid The id of the snap
     */
    void ExecuteSnapCmdL( CSmlDmBmCommandElement& aCmd, TUint aLuid );

    /**
     * Executes iap list commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aSnap The snap object
     */
    void ExecuteIapsCmdL( CSmlDmBmCommandElement& aCmd, 
                          RCmDestinationExt& aSnap );

    /**
     * Executes protection commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aSnap The snap object
     */
    void ExecuteProtectionCmdL( CSmlDmBmCommandElement& aCmd, 
                                RCmDestinationExt& aSnap );

    /**
     * Executes hidden commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aSnap The snap object
     */
    void ExecuteHiddenCmdL( CSmlDmBmCommandElement& aCmd, 
                            RCmDestinationExt& aSnap );

    /**
     * Executes metadata commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aSnap The snap object
     */
    void ExecuteMetadataCmdL( CSmlDmBmCommandElement& aCmd, 
                              RCmDestinationExt& aSnap );

    /**
     * Executes embedded snap commands. The valid commands include 
     * update, get and delete. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aLuid The id of the snap
     */
    void ExecuteEmbeddedSnapCmdL( CSmlDmBmCommandElement& aCmd, 
                                  RCmDestinationExt& aSnap );
    /**
     * Executes name commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aSnap The snap object
     */
    void ExecuteNameCmdL( CSmlDmBmCommandElement& aCmd, 
                          RCmDestinationExt& aSnap );


private: //data

    /**
     * CmManager is used for accessing CommsDat.
     */
     RCmManagerExt iCmManagerExt;
     
    /**
     * Pointer to the adapter. This is used for 
     * querying data from DM framework when necessary
     * and using helper functions. Not own.
     */ 
    CNSmlDmBmAdapter* iAdapter;

  };


#endif // NSmlDmBmSettingStore_H
