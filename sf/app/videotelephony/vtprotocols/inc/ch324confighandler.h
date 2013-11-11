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
* Description:  
*
*/


#ifndef CH324ConfigHandler_H
#define CH324ConfigHandler_H
NONSHARABLE_CLASS( TH324ConfigHandler ) : public MPVH324MConfigSymbianObserver
    {
    public:
        
        /**
         * Constructor.
         */
        TH324ConfigHandler(MVtProtocolHandler* aProtocolHandler);

        /**
        Handle an event that has been generated.

        @param aResponse  The response to a previously issued command.
        */
        virtual void MPVH324MConfigCommandCompletedL(const CPVCmdResponse& aResponse);

        /**
        Handle an information event.

        @param aEvent  
        */
        virtual void MPVH324MConfigInformationalEventL(const CPVAsyncInformationalEvent& aEvent);
    private:
        MVtProtocolHandler* iProtocolHandler;
    };
 
NONSHARABLE_CLASS( CH324ConfigCommand ): public CBase,
                                         public MVtH324ConfigCommand
    {
    public:
        /**
         * Destructor.
         */
        ~CH324ConfigCommand();

        /**
         * Sets the vendor identification data.  This does not cause the stack to issue a vendor identifiation request.
         * Set to NULL to disable sending vendor id.  If set to a valid parameter before Connect, it will cause the stack
         * to automatically send it along with the TCS message.
         * @param cc
         *         T35 Country code
         * @param ext
         *         T35 Extension
         * @param mc
         *         T35 Manufacturer code
         * @param aProduct
         *         Product number
         * @param aVersion
         *         Version number
         **/
        virtual TInt SetVendorId(TUint8 cc, TUint8 ext, TUint32 mc, const TDesC8* aProduct, const TDesC8* aVersion);

        /**
         * This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
         * It is a request to the remote encoder to adjust its encoding in accordance with the tradeoff value.
         * A value of 0 indicates a high spatial resolution and a value of 31 indicates a high frame rate.
         * The values from 0 to 31 indicate monotonically a higher frame rate. Actual values do not correspond
         * to precise values of spatial resolution or frame rate.
         *
         **/
        virtual TInt SendVideoTemporalSpatialTradeoffCommand(TUint aLogicalChannel, TUint8 aTradeoff);

        /**
         * This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
         * It is an indication to the remote decoder that the local encoder has adjusted its encoding parameters
         * according to the tradeoff value.
         * A value of 0 indicates a high spatial resolution and a value of 31 indicates a high frame rate.
         * The values from 0 to 31 indicate monotonically a higher frame rate. Actual values do not correspond
         * to precise values of spatial resolution or frame rate.
         *
         **/
        virtual TInt SendVideoTemporalSpatialTradeoffIndication(TUint aLogicalChannel, TUint8 aTradeoff);

        /**
         * This API allows the user to specify the supported resolutions for video for transmit and receive.
         *
         **/
        virtual TInt SetSupportedResolutions( );
        /**
        * This API allows the user to set options for fast call setup procedures
        **/
        virtual TInt SetFastCsupOptions( );

        /**
         * Causes the protocol to send the specified user input to the remote terminal using
         * control channel.  The user input can be either DTMF ot Alphanumeric
         * @param user_input A pointer to MVTUserInput
         * @returns A unique command id for asynchronous completion
         **/
        virtual TInt SendUserInputMessageL(MVTUserInput& user_input);

        /**
         * This API allows the user to specify observers for the 324m interface.
         *
         * @param aHandler     the observer for command status and unsolicited informational events
         **/
        virtual void SetObserverL(MVtProtocolHandler* aHandler);
       
       /**
        * Get member variant.
        */
        MPVH324MConfig*& PVH324ConfigInterface() { return iH324Config; }
    private:
        MPVH324MConfig* iH324Config;
        TH324ConfigHandler* iH324ConfigHandler;
    };    
#endif
//  End of File
 
