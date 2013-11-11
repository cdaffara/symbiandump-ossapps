/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The container class for ConnTest
* Shows status line and notification text editor
*
*/

#ifndef CONNTESTCONTAINER_H
#define CONNTESTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "uinotify.h"


// FORWARD DECLARATIONS
class CRichText;
class CParaFormatLayer;
class CCharFormatLayer;
class CEikRichTextEditor;
class CEikLabel;


// CLASS DECLARATION

/**
*  CConnTestContainer  container control class.
*  
*/
class CConnTestContainer : public CCoeControl, public MCoeControlObserver, public MUINotify
{
public: // Constructors and destructor
    
    /**
    * EPOC default constructor.
    * @param aRect Frame rectangle for container.
    */
    void ConstructL(const TRect& aRect);
    
    /**
    * Destructor.
    */
    ~CConnTestContainer();
    
public: // New functions
    
    /**
    * Clear the output window.
    */
    void ClearOutputWindow();
    
    /**
     * Stores the content of output window to a file.
     */
    void SaveOutputWindowToFile();
    
public: // Functions from base classes
    
    // From MUINotify

    /**
    * Display text on output window
    * @param aDes text to display
    * @param aFontStyle style (italic/bold/etc) for this text
    */
    void PrintNotify(const TDesC& aDes, TUint aFontStyle = 0);
    void PrintNotify(const TDesC8& aDes, TUint aFontStyle = 0);
    
    /**
    * Display integer as text on output window
    * @param aInt integer to display
    */
    void PrintNotify(TInt aInt);
    
    /**
    * Notify user of an error
    * @param aErrMessage message associated with error
    * @param aErrCode error code
    */
    void ErrorNotify(const TDesC& aErrMessage, TInt aErrCode);

    /**
    * Notify user with a message
    * @param aMessage message associated with error
    */
    void CConnTestContainer::PopupNotify(const TDesC& aMessage);
    
    /**
    * Change 'status' display.
    * @param aStatus new status text to display
    */
    void SetStatus(const TDesC& aStatus);
        
private: // Functions from base classes
    
    /**
    * From CoeControl. Calls SizeChangedL with TRAP.
    */
    void SizeChanged();  
    
    /**
    * From CoeControl.
    */
    TInt CountComponentControls() const;
    
    /**
    * From CCoeControl.
    */
    CCoeControl* ComponentControl(TInt aIndex) const;
    
    /**
    * Event handling section, e.g Listbox events.
    */
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    /**
    * From CCoeControl,Draw.
    */
    void Draw(const TRect& aRect) const;
    
    /**
    * Key event handler. Handles up and down arrow keys, so that
    * output window can be scrolled.
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    
    /**
    * Change font style for subsequent text
    * @param aFontStyle new font style
    */
    void SetFontStyle(TUint aFontStyle);
    
    /**
    * Set or clear an individual font attribute if necessary,
    * so that it matches the required style
    * @param aFontStyle required font style
    * @param aAttribute the individual attribute to correct
    */
    void UpdateFontAttribute(TUint aFontStyle, TInt aAttribute);
    
    
private: //data
    CParaFormatLayer*   iParaFormat;   // Used by CEikRichTextEditor 
    CCharFormatLayer*   iCharFormat;   // Used by CEikRichTextEditor 
    CRichText*          iRichText;     // Used by CEikRichTextEditor 
    CEikRichTextEditor* iOutputWindow; // CRichText object for use as our output window
    CEikLabel*          iStatusWindow; // Single line of text to display connection status
    TUint               iFontStyle;    // Current style in use by output window
    };
    
#endif
    
    // End of File
