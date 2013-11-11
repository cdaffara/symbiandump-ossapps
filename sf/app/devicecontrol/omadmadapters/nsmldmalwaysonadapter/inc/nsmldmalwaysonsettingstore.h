/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manages always-on settings in Central Repository.
*
*/


#ifndef NSMLDMALWAYSONSETTINGSTORE_H
#define NSMLDMALWAYSONSETTINGSTORE_H

#include "nsmldmalwaysonadapter.h"

#include <centralrepository.h>

const TInt KDefaultLuid = 1;

class CNSmlDmAOAdapter;
class CSmlDmAOCommandElement;

/**
 *  Setting Store is used for managing settings in CentralRepository
 *
 *  The class provides functions for executing commands 
 *  and sets of commands that involve manipulation of 
 *  data in CentralRepository. 
 *
 *  @lib nsmldmalwaysonadapter
 *  @since S60 v3.2
 */
class CNSmlDmAOSettingStore : public CBase
    {

public:

    static CNSmlDmAOSettingStore * NewL( CNSmlDmAOAdapter* aAdapter );

    virtual ~CNSmlDmAOSettingStore();

    /**
     * Executes the argument command. 
     *
     * @since S60 v3.2
     * @param aCmd Command to be executed
     * @param aLUID LUID of the VENDORCONFIG which command is referring.
     */
     void ExecuteCmdL( CSmlDmAOCommandElement& aCmd, TUint aLuid);   

    /**
     * Fetches all VENDORCONFIGs and inserts id of each VENDORCONFIG
     * into the argument array (in practise there is only one VENDORCONFIG)
     *
     * @since S60 v3.2
     * @param aLUIDArray Array for VENDORCONFIG ids
     */
    void GetVendorConfigsL( RArray<TUint32>& aLUIDArray );
 
private:

    CNSmlDmAOSettingStore( CNSmlDmAOAdapter * aAdapter );

    void ConstructL();

//----------- Commands for managing leaf nodes -------------------//

    /**
     * Executes VENDORCONFIG commands. The valid commands include 
     * get. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aLuid The id of the VENDORCONFIG
     */
    void ExecuteVendorConfigCmdL( CSmlDmAOCommandElement& aCmd, TUint aLuid );    

    /**
     * Executes NAME commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd    The command to be executed
     * @param aCenrep Pointer to PdpContextManager2 repository
     */
    void ExecuteNameCmdL( CSmlDmAOCommandElement& aCmd, 
                          CRepository*            aCenrep );


    /**
     * Executes AWON-PDPC commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aCenrep Pointer to PdpContextManager2 repository
     */
    void ExecuteAwonPdpcCmdL( CSmlDmAOCommandElement& aCmd, 
                              CRepository*            aCenrep );


    /**
     * Executes T-RETRY commands. The valid commands include 
     * update and get. With update commands data is read from the 
     * iData memeber of the argument command. With get commands the iData 
     * memeber is used to pass fetched data back to caller. 
     * After execution of command the function fills in the status 
     * in aCmd.iStatus memeber and sets aCmd.iExecuted as True. The function
     * leaves if command can not be executed for some reason. 
     *
     * @param aCmd The command to be executed
     * @param aCenrep Pointer to PdpContextManager2 repository
     */
    void ExecuteTRetryCmdL( CSmlDmAOCommandElement& aCmd, 
                            CRepository*            aCenrep );
    
     /**
     * Parses integer from descriptor 
     *
     * @param aPtr Descriptor containing value
     * @param aInt Value as integer
     */                        
    void ParseIntegerL( const TDesC& aPtr, TInt& aInt );
    
     /**
     * Parses settings for home network and visited network 
     *
     * @param aInt Interger containing home & visited network setting
     * @param aHplmn "1" when always-on enabled in hplmn, otherwise "0"
     * @param aVplmn "1" when always-on enabled in vplmn, otherwise "0"
     */
    void ParseAwonPdpcValuesL( const TInt aInt, TInt& aHplmn, TInt& aVplmn );                        

private: //data
     
    /**
     * Pointer to the adapter. This is used for 
     * querying data from DM framework when necessary
     * and using helper functions. Not own.
     */ 
    CNSmlDmAOAdapter* iAdapter;

  };


#endif // NSMLDMALWAYSONSETTINGSTORE_H
