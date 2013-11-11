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
* Description: This file contains CTestOutputListBoxModel class
* declaration.
*
*/

#ifndef TESTCASEOUTPUTCONTAINER_H
#define TESTCASEOUTPUTCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>
#include <bamdesca.h>   // MDesCArray

// #include "RRefArray.h"  
#include <stifinternal/RRefArray.h>


// Define *.mbm file name.
_LIT(KMbmFileName, "\\Resource\\Apps\\Stifui.mbm");

// FORWARD DECLARATIONS
class CEikTextListBox;
class CAknSingleGraphicStyleListBox;
class CDesC16ArrayFlat;
class CStartedTestCase;

class CUIStore;

// CLASS DECLARATION

class CTestOutputListBoxModel : public CBase, public MDesCArray
    {
    public:
        /**
        * Destructor.
        */
        ~CTestOutputListBoxModel();
        
        /**
        * Two-phased constructor.
        * @param aStartedTestCase Pointer to started test case.
        * @return Pointer to created list box model.
        */
        static CTestOutputListBoxModel* NewL(CStartedTestCase* aStartedTestCase);

    public: // Functions from base classes
    
        /**
        * Returns the number of descriptor elements in the array.
        * @return The number of descriptor elements.
        */
        TInt MdcaCount() const;
        
        /**
        * Indexes into a descriptor array.
        * @param aIndex The position of the descriptor element within the
        *               array. The position is relative to zero; i.e. zero
        *               implies the first descriptor element in the array. 
        *               This value must be non-negative and less than the 
        *               number of descriptors currently within the array 
        *               otherwise the operator panics with 
        *               EArrayIndexOutOfRange. 
        * @return A non-modifiable pointer descriptor representing the 
        *         descriptor element located at position aIndex within 
        *         the array.
        */
        TPtrC MdcaPoint(TInt aIndex) const;

    private:
        /**
        * Symbian OS two phased constructor.
        * Completes the construction of the object.
        */
        void ConstructL();

    private: //data
        CStartedTestCase* iStartedTestCase;
        HBufC*            iBuffer;

    };

/**
*  CTestCaseOutputContainer  container control class.
*  
*/
class CTestCaseOutputContainer : public CCoeControl, public MEikListBoxObserver /* MCoeControlObserver */
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        * @param aRect Frame rectangle for container.
        */
        //void ConstructL(const TRect& aRect, TInt aExecutedTestCaseCount );
        void ConstructL(const TRect& aRect, CStartedTestCase* aStartedTestCase );

        /**
        * Destructor.
        */
        ~CTestCaseOutputContainer();

    public: // New functions
        /**
        * Handles addition of item to list box.
        */
        void OutputUpdateL();

    private:

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
        * Handles list box events.
        * @param aListBox The originating list box. 
        * @param aEventType A code for the event. Further information may be
        *                   obtained by accessing the list box itself.
        */
        void HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType);

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
        
        CEikTextListBox*            iListBox;
        CTestOutputListBoxModel*    iListBoxModel;
        CUIStore*                   iUIStore;

    };

#endif // TESTCASEOUTPUTCONTAINER_H

// End of File
