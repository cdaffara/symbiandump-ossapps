// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __THELPVIEWCTRLS_H__
#define __THELPVIEWCTRLS_H__

// System includes
#include <coecntrl.h>
#include <coecobs.h>
#include <techview/eiktxlbx.h>
#include <techview/eiktxlbm.h>

// Classes referenced
class CHlpList;
class CHlpItem;
//
class CEikEdwin;
class CEikCommandButton;



//
// ----> CHlpSearchPrompt (header)
//
class CHlpSearchPrompt : public CCoeControl, public MCoeControlObserver
    {
//
public:
//
	CHlpSearchPrompt();
	~CHlpSearchPrompt();
	void ConstructL(CCoeControl& aParent);

//
public:	// NEW
//
	void ResetSearchL();
	HBufC* SearchTextLC();

//
public:	// FROM CCoeControl
//
	void MakeVisible(TBool aVisible);
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

//
public:	// FROM MCoeControlObserver
//
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

//
private: // NEW INTERNAL
//
	static TInt HandleTimerExpiredL(TAny* aSelf);

//
private: // MEMBER DATA
//
	CEikEdwin* iEdwin;
	CPeriodic* iHideTimer;
    };






//
// ----> CHlpListBox (header)
//
class CHlpListBox : public CCoeControl, public MCoeControlObserver
    {
//
public:
//
	CHlpListBox();
	~CHlpListBox();
	void ConstructL(CCoeControl& aParent);

//
public:	// FROM CCoeControl
//
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	TSize MinimumSize();

//
public:	// NEW
//
	const CHlpItem& HelpItemL() const;
	void PopulateListBoxL(CHlpList* aContent); // takes ownership
	inline CTextListBoxModel* Model() const		{ return iListBox->Model(); }
	inline CEikTextListBox* ListBox() const		{ return iListBox; }

//
public:	// FROM MCoeControlObserver
//
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

//
private: // INTERNAL
//
	void SetContent(CHlpList* aContent);

//
private: // MEMBER DATA
//
	CHlpList* iContent;
	CEikCommandButton* iButton;
	CEikTextListBox* iListBox;
	};









#endif
