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
* Description:  Interface class for ui customization.
*
*/

#ifndef MPHONECUSTOMIZATION_H
#define MPHONECUSTOMIZATION_H

class CEikMenuBar;
class CEikMenuPane;
class TPhoneCmdParamCallHeaderData;


// CLASS DECLARATION
/**
*  Interface class for ui customization.
*/
class MPhoneCustomization
    {
public:
    /**
    * Customizes menu bar.
    * @param aResuorceId Menu bar resource id
    * @param aMenuBar Menu bar.
    */
    virtual void CustomizeMenuBarL( TInt aResourceId, 
                                    CEikMenuBar* aMenuBar) = 0;
    
    /**
    * Customizes menu pane.
    * @param aResuorceId Menu pane resource id
    * @param aMenuPane Menu pane.
    */
    virtual void CustomizeMenuPaneL( TInt aResourceId, 
                                     CEikMenuPane* aMenuPane) = 0;

    /**
    * Handles custom menu commands.
    * @param aCommand Command id.
    * @return ETrue if command handled.
    */
    virtual TBool HandleCommandL( TInt aCommand ) = 0;

    /**
    * Checks is alphanumeric mode allowed for number entry/
    * dialer.
    * @return ETrue if allowed.     
    */
    virtual TBool AllowAlphaNumericMode() = 0; 
    
    /**
    * Returns resource id for soft keys.
    * @return Resource id.
    */
    virtual TInt CustomizeSoftKeys() = 0;
    
    /**
    * Returns text resource id for private call header.
    * @return Resource id.
    */
    virtual TInt CustomizeCallHeaderText() = 0;
    
    /**
    * Customizes call header texts shown in a call bubble.
    * @param    aCallId             Call identifier.
    * @param    aHeaderData         Call header data to be modified.
    * @param    aInCallNumberText   In call number text (Call 1, Call 2...).
    */
    virtual void ModifyCallHeaderTexts( TInt aCallId, 
        TPhoneCmdParamCallHeaderData* aHeaderData,
        const TDesC& aInCallNumberText ) = 0;
    
    /**
    * Returns text resource id for busy note.
    * @return Resource id.
    */
    virtual TInt CustomizeBusyNoteText() = 0;

    /**
    * Handles calls from number entry.
    * @return ETrue when call is handled by customization.
    *         EFalse otherwice.
    */
    virtual TBool HandleCallFromNumberEntryL() = 0;

    /**
    * A message handling function for Phone Engine messages
    * @param aMessage   Id of the message 
    * @param aCallId    the call id of the call
    */
    virtual TBool HandlePhoneEngineMessageL(
        const TInt aMessage, 
        TInt aCallId ) = 0; 
        
    /**
    * Handles dial by unknown service id.
    * @param aNumber   Number/Address 
    */
    virtual void HandleDialL( const TDesC& aNumber ) = 0; 
        
    };
    
#endif // MPHONECUSTOMIZATION_H
    
