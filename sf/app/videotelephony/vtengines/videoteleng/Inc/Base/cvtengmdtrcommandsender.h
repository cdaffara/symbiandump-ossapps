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
* Description:  Sender class for mediator commands
*
*/


#ifndef C_VTENGMDTRCOMMANDSENDER_H
#define C_VTENGMDTRCOMMANDSENDER_H

//  INCLUDES

#include <e32cmn.h>
#include <MediatorCommandInitiator.h>
#include "MVtEngEventObserver.h"

// FORWARD DECLARATIONS
class CVtEngUtility;
class CVtEngOperation;
class CVtEngMdtrCommandSender;
class TVtMdtrStateIdle;
// CLASS DECLARATION

/**
*  Mediator command sender 
*
*  @lib videoteleng
*  @since 5.0
*/
NONSHARABLE_CLASS( CVtEngMdtrCommandSender ) :
    public CBase, 
    public MMediatorCommandResponseObserver,
    public MVtEngEventObserver
    {
public: // constructor and destructors

    /**
    * two-phase constructor
    */
    static CVtEngMdtrCommandSender* NewL( CVtEngUtility& aUtils );
    
    // Destructor
    ~CVtEngMdtrCommandSender();

public: // new methods

    /**
     * Requests telephony to end video call
     */
    void EndCall();

    /**
     * Requests telephony to fallback to voice call 
     */
    void VoiceFallback();
    
    /**
     * Requests telephony to make voice call 
     */
    void SwitchToVoiceL( CVtEngOperation& aOperation );
    
    /**
     * Notifies telephony about outgoing audio state change
     */
    void NotifyOutgoingAudioState( const TBool aMicEnabled );
    
    void SetSenderState( TVtMdtrStateIdle& aNewState );
        
    
public: // from MMediatorCommandResponseObserver

        // responses are not handled but required by mediator api
    virtual void CommandResponseL( TUid aDomain,
           TUid aCategory, 
           TInt aCommandId,
           TInt aStatus, 
           const TDesC8& aData );

public: // from MVtEngEventObserver

    // handles event accordint to ongoing operation
    virtual void HandleVtEventL( TInt aEvent );
                         
private: // constructors

    CVtEngMdtrCommandSender( CVtEngUtility& aUtils );
    
    void ConstructL();

private:
    // for switch to voice operation, signaled when mediator command is issued.
    CVtEngOperation* iOperation;
        
    // for mediator command reception
    CMediatorCommandInitiator* iMediatorInitiator;
    
    CVtEngUtility& iUtils;
    
    TVtMdtrStateIdle* iSenderState;
    };

#endif // C_VTENGMDTRCOMMANDSENDER_H

// End of File
