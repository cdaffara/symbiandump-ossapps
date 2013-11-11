/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the class CPEGsmAudioDatar, 
*                which is an GSM specific interface class to audio properties of the system
*
*/


#ifndef CPEGSMAUDIODATA_H
#define CPEGSMAUDIODATA_H

//  INCLUDES
#include "cpeaudiodata.h"
#include <pevirtualengine.h>
#include <e32base.h>
#include <mccedtmfobserver.h>

// CONSTANTS
// None.

// MACROS
// None.


// DATA TYPES
// None.


// FUNCTION PROTOTYPES
// None.


// FORWARD DECLARATIONS
class CPEAudioInbandTonePlayer;
class CPEAudioFactory;
class MPEClientInformation;

// CLASS DECLARATION

/**
*  This file contains the header file of the class CPEGsmAudioData, 
*
*  @lib Audiohandling.lib
*  @since Series60_2.7
*/
class CPEGsmAudioData : public CPEAudioData,
                        public MCCEDtmfObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPEGsmAudioData* NewL( MPEPhoneModelInternal& aPhoneModel,
                                               RFs& aFsSession );                                  
           
                                               
        /**
        * Two-phased constructor for testing purposes
        */        
        IMPORT_C static CPEGsmAudioData* NewL( MPEPhoneModelInternal& aPhoneModel,
                                               RFs& aFsSession,
                                               CPEAudioFactory& aAudioFactory );                          
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPEGsmAudioData();
        
    public: // From MCCEDtmfObserver
                                       
        /**
        * All the occurred DTMF events are notified through this interface
        *
        * @since S60 3.2
        * @param aEvent Event code
        * @param aError Error code
        * @param aTone tone 
        * @return none
        */
        IMPORT_C void HandleDTMFEvent( const MCCEDtmfObserver::TCCEDtmfEvent aEvent, 
                                       const TInt aError, 
                                       const TChar aTone );

    public: // New functions

        /**
        * Plays the given dtmf tone
        * @since Series60_2.7
        * @param  aTone, dtmf tone to be played
        * @param  aVolume, dtmf tones volume
        * @return None.  
        */
        IMPORT_C void PlayDtmfTone( const TChar& aTone ) const; 

        /**
        * Stops the playing of dtmf tone
        * @since Series60_2.7
        * @param  None.
        * @return None.  
        */
        IMPORT_C void StopDtmfTonePlay(); 

        /**
        * Plays given tone.
        * @since Series60_2.7
        * @param aToneName Tone sequence's name to be played.
        * @return None.
        */
        IMPORT_C void PlayInbandTone();

        /**
        * Stops the playing of inband tone
        * @since Series60_2.7
        * @param  None.
        * @return None.  
        */
        IMPORT_C void StopInbandTonePlay(); 

    protected:  // From CPEAudioData
        
        void DoHandleVolumeChange( TInt aVolume, TBool aSendMsg=EFalse );
    
    private:
        /**
        * C++ default constructor.
        */
        CPEGsmAudioData( MPEPhoneModelInternal& aPhoneModel,
                         RFs& aFsSession );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CPEAudioFactory& aAudioFactory);

        /**
        * Checks if a DTMF tone may be played locally
        * The check is performed to prevent local play of dtmf tones sent by SAT application
        * during a SAT originated call
        *
        * @return ETrue: local play is allowed,
        *         EFalse: local play is not allowed
        */
        TBool DtmfLocalPlayAllowed() const;

    private:    //Data
    
        // CPEAudioDtmfTonePlayer is called to play the dtmf tones
        CPEAudioDtmfTonePlayer* iDtmfTonePlayer;
        // CPEAudioInbandTonePlayer is called to play the inband tones
        CPEAudioInbandTonePlayer* iInbandTonePlayer;
        // handle to a file server session
        RFs& iFsSession;

    };

#endif      //CPEGSMAUDIODATA_H  
            
// End of File
