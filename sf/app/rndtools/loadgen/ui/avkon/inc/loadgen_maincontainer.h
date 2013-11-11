/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#ifndef LOADGEN_VALUESCONTAINER_H
#define LOADGEN_VALUESCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h> 
#include <aknlists.h> 

// FORWARD DECLARATIONS
class CLoadGenModel;
class CLoadGenCAknSingleGraphicStyleListBox;

// CLASS DECLARATIONS

class CLoadGenMainContainer : public CCoeControl, MCoeControlObserver, MEikListBoxObserver
    {
public:
    void ConstructL(const TRect& aRect);
    ~CLoadGenMainContainer();

private:
    void SizeChanged();  
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void HandleResourceChange(TInt aType);
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);  // From MEikListBoxObserver
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);  // From MCoeControlObserver
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public:
    const CArrayFix<TInt>* ListBoxSelectionIndexes();
    TInt ListBoxSelectionIndexesCount();
    void SetDefaultTitlePaneTextL();
    TInt CurrentListBoxItemIndex();
    void SetListBoxTextArrayL(CDesCArray* aTextArray);
    inline CLoadGenCAknSingleGraphicStyleListBox* ListBox() { return iListBox; }

private:
    CLoadGenModel*                              iModel;
    CLoadGenCAknSingleGraphicStyleListBox*      iListBox;
    };

class CLoadGenCAknSingleGraphicStyleListBox : public CAknSingleGraphicStyleListBox
    {
    void SizeChanged();
    void SizeChangedL();
    };
        
#endif

// End of File
