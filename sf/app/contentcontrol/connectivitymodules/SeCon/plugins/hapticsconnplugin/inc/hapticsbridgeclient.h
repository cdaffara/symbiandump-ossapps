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
* Description:  Client interface to haptics server for haptics bridge 
*                commands.
*
*/


#ifndef C_HAPTICSBRIDGECLIENT_H
#define C_HAPTICSBRIDGECLIENT_H

#include <e32base.h>

class CHWRMHapticsPacketizer;

class RHapticsBridgeClient : public RSessionBase
{
public:
    /**
     * Constructor
     */
     RHapticsBridgeClient();
     
     /**
      * Destructor
      */
     virtual ~RHapticsBridgeClient();

public:
    /**
     * Create a connection to haptics server.
     */
	TInt Connect();

    /**
     * Close the connection to haptics server.
     */
	void Close();

    /**
     * Send message to haptics server.
     * @param aReqData     Data request to be sent to haptics server.
     * @param aRetDataPckg Returned data from haptics server.
     * @return KErrNone if sent succesfully, otherwise system wide error code.
     */
	TInt SendBridgeBuffer( const TDesC8& aReqData, TDes8& aRetDataPckg );

    /**
     * Sends a message clean-up commend to haptics server.
     */
	void CleanUp();
    

private:

    /**
     * Returns the version of the haptics server.
     * @return Version.
     */
	TVersion ServerVersion() const;

    /**
     * Starts the server process if it is not already running.
     * @return KErrNone on success, otherwise a system error code.
     */
	TInt StartServer() const;

    /**
     * Handles the device opening request.
     * @param aResponse Reference to a descriptor containing the response
     *                  received from haptics server.
     */
	void HandleOpenDeviceResponseL( const TDesC8& aResponse );
	
	/**
	 * Opens the haptic effect ("vibra") device.
	 * @return KErrNone on success, otherwise a system error code.
	 */
    TInt OpenHapticsDevice();

private:

    /**
     * Haptic packetizer.
     * Owned.
     */
    CHWRMHapticsPacketizer* iPacketizer;     
};

#endif // C_HAPTICSBRIDGECLIENT_H