/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class of PhoneEngineGsm
*
*/


#ifndef C_CPEPHONEMODELIF_H
#define C_CPEPHONEMODELIF_H

#include <e32base.h>
#include <pevirtualengine.h>
#include <mpeengineinfo.h>

class MEngineMonitor;
class CPEAudioFactory;

/**
 * Offers message interface from phone application GSM to phone engine GSM.
 *
 *  @lib phoneenginegsm
 *  @since S60 v.4.0
 */
NONSHARABLE_CLASS( CPEPhoneModelIF ) : public CBase,
                           public MPEPhoneModel
    {

public:

    typedef TInt TPEMessagesToPhoneEngineGsm;

    enum
        {
        // Audio messages                         11000 - 11199

        // Call messages                          11200 - 11399
        EPEMessageAddConferenceMember           = KPEMessagesToPhoneEngineGsmFirst + 200,
        EPEMessageCreateConference,
        EPEMessageDropConferenceMember,
        EPEMessageGoOneToOne,
        EPEMessageHangUpConference,
        EPEMessageHold,
        EPEMessageHoldConference,
        EPEMessageResume,
        EPEMessageResumeConference,
        EPEMessageSwap,
        EPEMessageSwapConference,
        EPEMessageTransfer,
                
        // Contact messages                       11400 - 11599

        // Error messages                         11600 - 11799

        // Network messages                       11800 - 12999

        // Parser messages                        12000 - 12199
        EPEMessageCancelSSstringCommand         = KPEMessagesToPhoneEngineGsmFirst + 1000, // Start last SS string command canceling

        // Security messages                      12200 - 12399

        // Settings messages                      12400 - 12599
        EPEMessageSetALSLine                    = KPEMessagesToPhoneEngineGsmFirst + 1400,
        // Miscellaneous messages                 12600 - 12799

        // Last                                   12999
        // Exposing this allows the compiler to detect the overlapping enumerations.
        EPEMessageLastVariantItem               = KPEMessagesToPhoneEngineGsmLast
        };

     ~CPEPhoneModelIF();

    /**
     * Initiates Phone Engine construction
     *
     * @since S60 v.4.0
     * The construction of Phone Application Engine is an asynchronous operation.
     * The Engine Monitor observer will be notified using the message interface when
     * the operation completes:
     *      message EPEMessagePEConstructionReady signifies a successful procedure
     *      message EPEMessagePEConstructionFailed signifies a failed procedure
     *
     * @param aEngineMonitor reference to Engine Monitor instance
     * @return pointer to protocol specific phone model object
     */
     IMPORT_C static CPEPhoneModelIF* CreateL( MEngineMonitor& aEngineMonitor );
     
     /**
      * Initiates Phone Engine construction for Module testing
      *
      * @since S60 v.4.0
      * The construction of Phone Application Engine is an asynchronous operation.
      * The Engine Monitor observer will be notified using the message interface when
      * the operation completes:
      *      message EPEMessagePEConstructionReady signifies a successful procedure
      *      message EPEMessagePEConstructionFailed signifies a failed procedure
      *
      * @param aEngineMonitor reference to Engine Monitor instance
      * @param aAudioFactory Factory to create audio related classes.
      * @return pointer to protocol specific phone model object
      */
      IMPORT_C static CPEPhoneModelIF* CreateL( 
          MEngineMonitor& aEngineMonitor,
          CPEAudioFactory& aAudioFactory );
     
protected:
    
    CPEPhoneModelIF();
          

    };
          
#endif // C_CPEPHONEMODELIF_H
