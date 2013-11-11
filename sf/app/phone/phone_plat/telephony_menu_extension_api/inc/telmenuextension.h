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
* Description:  Defines interface for manipulating telephony menus using ECOM
*                plugin.
*
*/


#ifndef TELMENUEXTENSION_H
#define TELMENUEXTENSION_H

// INCLUDES
#include <e32base.h>
#include <teltouchpaneextension.h>

// CONSTANTS
// Constants used in ECOM implementation
const TUid KTelMenuExtensionInterfaceUid    = { 0x2001E2A0 };

// FORWARD DECLARATIONS
class CEikMenuPane;

/**
 * Plugin interface class for manipulating telephony
 * menu items.
 *
 * Menu customization *
 * Plug-in is loaded by using service table value for most active call.
 *
 * Command customization *
 * Plug-in can dimm existing commands or create new command. If default 
 * implementation doesnt handle command, plug-in is loaded and 
 * HandleCommandL is called.
 *
 * @since S60 5.1
 */
class CTelMenuExtension : public CBase
    {
public: // Data

    /** 
     * Specifies call types
     */    
    enum TCallType
        {
        EUninitialized = 0,
        ECsVoice = 1,  // Circuit switched voice call
        ECsFax = 2, // Fax call
        ECsData = 3, // Circuit switched data call
        ECsVideo = 4, // Multimedia/Video call
        EPsVoice = 5 // Voice over IP call
        };
        
    /** 
     * Specifies call states
     */
    enum TCallState
        {
        ENone = 0,
        EDisconnecting = 1, // Call is disconnecting
        EActive = 2, // Call is active
        EOnHold = 3, // Call is on hold
        EIncoming = 4, // Call is ringing
        EOutgoing = 5 // Call is dialling or connecting
        };
        
    /**
     * Capsulates call info.
     */
    class TCallInfo
        {
    public:
        // Call id, specified by phoneengine
        // Conference call can be determined by using this integer
        // Check phoneengine API
        TInt iCallId;
        
        // Service provider settings item Id
        TUint iServiceId;
        
        // Calls state
        TCallState iCallState;
        
        // Calls type
        TCallType iCallType;
        };


public:

    /**
     * Create new plugin having the given UID.
     *
     * @leave Uses Leave code KErrNotFound if implementation is not found.
     * @param aImplementationUid Implementation UID of the plugin to be
     *        created.
     */
    static CTelMenuExtension* NewL(
        TUid aImplementationUid );

    /**
     * Destructor
     */
    virtual inline ~CTelMenuExtension();

        
    /**
     * Modifys given menu pane. Removes unsupported options from call menu and 
     * adds custom menu commands to the menu.
     *
     * @param aCallArray - Array holding information of all ongoing calls.
     * @param aMenuPane - A modifyable menu pane.
     */
    virtual void ModifyExtensionsCallMenuL( 
                    const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
                    CEikMenuPane& aMenuPane ) = 0;

    /**
     * Modifys given touch pane buttons. Dimms unsupported buttons from the
     * pane.
     *
     * @param aCallArray - Array holding information of all ongoing calls.
     * @param aTouchPane - A touch pane interface.
     */
    virtual void ModifyExtensionsTouchPaneButtonsL( 
                    const RArray<CTelMenuExtension::TCallInfo>& aCallArray,
                    MTelTouchPaneExtension& aTouchPane ) = 0;

    /**
     * Handles custom menu commands.
     *
     * @param aCommand It is the code of the command to be handled.
     * @return boolean value was the command handled (ETrue) or not (EFalse).
     */
    virtual TBool HandleCommandL( TInt aCommand ) = 0;


private: // data

    /**
     * ECOM plugin instance UID.
     */
    TUid iDtor_ID_Key;
    };

#include "telmenuextension.inl"


#endif // TELMENUEXTENSION_H

//End of file
