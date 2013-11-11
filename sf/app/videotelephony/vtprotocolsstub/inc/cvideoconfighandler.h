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
* Description:  Video configuration interface
*
*/

#ifndef CVideoConfigHandler_H
#define CVideoConfigHandler_H
NONSHARABLE_CLASS( TVideoConfigHandler )
    {
    public:
        
        /**
         * Constructor.
         */
        TVideoConfigHandler(MVtProtocolHandler* aProtocolHandler);

    private:
        MVtProtocolHandler* iProtocolHandler;
    };

NONSHARABLE_CLASS( CVideoConfigCommand ) : public CBase,
                                           public MVtVideoConfigCommand
    {
    public:
        /**
         * Destructor.
         */
        ~CVideoConfigCommand();

        /**
         * Sets the I-Frame refresh rate of the encoded output.
         *
         * @param aIFrameInterval I-Frame rate in seconds per I-Frame
         * @return True if successful, else false
         */
        virtual TInt SetIntraFrameInterval(TUint32 aIFrameInterval);

        /**
         * Requests the encoder to encode the next frame as an I-Frame.  This menthod is valid
         * only when the node is in EPVMFNodeStarted state. If successful, the next encoded
         * frame will be an I-Frame.
         *
         * @return True for success, else false
         */
        virtual TInt RequestNextIntraFrame();
        
        /**
         * Sets the frame rate of encoded output for the specified layer.
         * @param aFrameRate Frame rate for the specified layer in frames per second.
         * @return True if successful, else false.
         */
        virtual TInt SetVideoFrameRate(TUint32 aFrameRate);

        /**
         * This API allows the user to specify separate observers for the extension interface.  
         * 
         * @param aHandler     the observer for unsolicited informational events
         **/
        virtual void SetObserverL(MVtProtocolHandler* aHandler);
                
    private:
        TVideoConfigHandler* iVideoConfigHandler;
    };
#endif
//  End of File 
