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
* Description: This file contains CStartCasesContainer class 
* declaration.
*
*/

#ifndef CSTARTCASESCONTAINER_H
#define CSTARTCASESCONTAINER_H

// INCLUDES
#include "Container.h"

#include <coecntrl.h>
#include <eiklbo.h> // MEikListBoxObserver
#include <bamdesca.h> // MDesCArray
#include <aknsfld.h>

//#include "RRefArray.h"        
//#include "UIEngine.h"         
//#include "UIStoreIf.h"        
//#include "UIStoreContainer.h" 

#include <stifinternal/RRefArray.h>
#include <stifinternal/UIEngine.h>
#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStoreContainer.h>
   
// FORWARD DECLARATIONS
class CEikTextListBox;
class CDesC16Array;

class CStartCasesView;
class CEikColumnListBox;
class CStartedTestCase;

class CUIStoreHandler;
class CUIStore;
class CUIStoreIf;
class CUIEngineContainer;
class CUIEngine;

// CLASS DECLARATION
/**
*  CStartCasesContainer  container control class.
*  
*/
class CStartCasesContainer : public CContainer//public CCoeControl /*, MEikListBoxObserver */ /* MCoeControlObserver */
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect, MEikListBoxObserver* aListBoxObserver);

        /**
        * Destructor.
        */
        ~CStartCasesContainer();

    public: // New functions
        
        /**
         * Stores current position of focus of iListBox to the CAppUiAppUi object
         */
        void SaveActiveLine();
        
    public: // Functions from base classes

    private: // Functions from base classes

        /**
        * From CoeControl,SizeChanged.
        * Called by framework when the view size is changed.
        */
        void SizeChanged();

        /**
        * From CoeControl,CountComponentControls.
        * Gets a count of the component controls of this list box control.
        * This information is used for DrawNow().
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl,ComponentControl.
        * Gets a pointer to the specified component control.
        * @param aIndex Index of the component control to look up.
        * @return The control at aIndex.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl,Draw.
        * Draw a control, called by window server.
        * @param aRect The region of the control to be redrawn. Co-ordinates
        *              are relative to the control's origin (top left corner).
        */
        void Draw(const TRect& aRect) const;
        
        /**
        * From CCoeControl,OfferKeyEventL
        * Handles key events.
        * @param aKeyEvent The key event.
        * @param aType The type of key event.
        * @return Indicates whether or not the key event was used 
        *         by this control.
        */
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
		        
    private: //data
        
        CStartCasesView*            iParentView;
    };

#endif // CSTARTCASESCONTAINER_H

// End of File
