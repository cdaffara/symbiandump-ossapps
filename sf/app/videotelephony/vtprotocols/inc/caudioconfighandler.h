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
* Description:  .
*
*/


#ifndef CAUDIOCONFIGHANDLER_H
#define CAUDIOCONFIGHANDLER_H
NONSHARABLE_CLASS( TAudioConfigHandler ) : public MPVAudioOutputControlObserver
    {
    public:
        /**
         * Constructor.
         */
        TAudioConfigHandler( MVtProtocolHandler* aProtocolHandler);

        /** 
        *  Signals completion of the audio output control command.
        *  @param aId The command id of the completed command.
        *  @param aCmd The command type.
        *  @param aContextData The context data passed in with the command.
        *  @param aStatus The command completion status.
        **/
        virtual void AudioOutputControlCommandComplete(TPVCommandId aId
                ,TPVAudioOutputControlCommand aCmd
                ,TAny *aContextData
                ,TInt aStatus);
    private:
        MVtProtocolHandler* iProtocolHandler;
    };
    
NONSHARABLE_CLASS( CAudioConfigCommand ) : public CBase,
                            public MVtAudioConfigCommand
    {
    public:
        
        /**
         * Constructor.
         */
        CAudioConfigCommand(MPVAudioOutputControl* aAudioCtrl);
        
        /**
         * Destructor.
         */
        ~CAudioConfigCommand();

        /**
         * Method to set the playback volume to the specified value.
         *
         * This is a pure virtual function that each derived class must implement.
         * It is also an asynchronous function which will be answered with a callback.
         * 
         * @param aNewVolume
         *        An input parameter to hold the value for the requested playback volume.  
         *
         * @returns   TInt
         *            Returns a command ID that can be used to identify a command completion result with corresponding request.
         */
        virtual TInt SetAudioVolumeL(TInt aVolume);

        /**
         * Method to get the maximum valid value for the playback volume.
         *
         * This is a pure virtual function that each derived class must implement.
         * It is also an asynchronous function which will be answered with a callback.
         * 
         * @param aMaxVolume
         *        An output parameter to hold the value for the maximum valid playback volume.  
         *            Note that the parameter should not be used until the callback indicates that the 
         *            method has completed.
         *
         * @returns   TInt
         *            Returns a command ID that can be used to identify a command completion result with corresponding request.
         */
        virtual TInt GetMaxAudioVolumeL(TInt& aMaxVolume);

        /**
         * This API allows the user to specify observers for the 324m interface.
         *
         * @param aHandler     
         **/
        virtual void SetObserverL(MVtProtocolHandler* aHandler);
    private:
        MPVAudioOutputControl* iAudioCtrl;
        TAudioConfigHandler* iAudioConfigHandler;
    };
#endif
//  End of File
