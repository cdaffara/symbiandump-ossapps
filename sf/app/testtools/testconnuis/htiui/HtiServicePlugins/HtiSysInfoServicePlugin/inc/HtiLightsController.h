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
* Description:  For controlling S60 device lights.
*                Helper for SysInfoServicePlugin.
*
*/


#ifndef HTILIGHTSCONTROLLER_H
#define HTILIGHTSCONTROLLER_H

// INCLUDES
#include <e32base.h>
#include <hwrmlight.h>

// FORWARD DECLARATIONS
class MHtiDispatcher;

// CLASS DECLARATION
/**
* For controlling S60 device lights. Helper class for SysInfoServicePlugin.
*/
class CHtiLightsController : public CBase,
                             public MHWRMLightObserver
    {
public:

    static CHtiLightsController* NewL( MHtiDispatcher* aDispatcher );

    /**
    * Called by the plugin when there is a message to be processed by
    * the lights controller.
    * @param aMessage message body destinated to the servive
    * @param aReply on return contains the reply message (OK response)
    */
    void ProcessMessageL( const TDesC8& aMessage, TDes8& aReply );

    ~CHtiLightsController();

    // from MHWRMLightObserver
    void LightStatusChanged( TInt aTarget,
                             CHWRMLight::TLightStatus aStatus );

protected:

    CHtiLightsController( MHtiDispatcher* aDispatcher );
    void ConstructL();

private: // private helper methods

    void HandleLightStatusL( const TDesC8& aMessage );
    void HandleLightOnL( const TDesC8& aMessage );
    void HandleLightOffL( const TDesC8& aMessage );
    void HandleLightBlinkL( const TDesC8& aMessage );


private: // private data

    // The message dispatcher interface for dispatching error messages.
    MHtiDispatcher* iDispatcher;

    // The reply message to dispatch.
    TBuf8<4> iReply;

    // Light client session.
    CHWRMLight* iLight;

    // The latest command
    TInt iCommand;

    // The light target bitmask of latest call
    TInt iTarget;

    // The duration given in latest call
    TInt iDuration;

    // The lights on duration given in latest blink lights call
    TInt iOnDuration;

    // The lights off duration given in latest blink lights call
    TInt iOffDuration;

    // The intensity value of latest call
    TInt iIntensity;

    // The fade value of latest call
    TBool iFade;
    };

#endif // HTILIGHTSCONTROLLER_H
