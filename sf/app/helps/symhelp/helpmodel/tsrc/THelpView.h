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

#ifndef __THELPVIEW_H__
#define __THELPVIEW_H__

// System includes
#include <coecntrl.h>
#include <coecobs.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include "hlpmodel.h"
#include <techview/eikon.hrh>

// Constants
const TUid KUidHelpViewApp		= { 0x100068D3 };
const TInt KNumberOfZoomStates	= 3;

// Classes referenced
class CHlpModel;
class CEikRichTextEditor;
class CEikChoiceList;
class CHlpAppUi;
// 
class CHlpListBox;
class CHlpSearchPrompt;


//
// ----> MZoomableControl (header)
//
class MZoomableControl
	{
//
public: // ZOOM ENUM
//
	enum TZoomEvent
		{
		EEventZoomIn = EEikCmdZoomIn,
		EEventZoomOut = EEikCmdZoomOut,
		EEventZoomDefault
		};

//
public:
//
	virtual void HandleZoomEventL(TZoomEvent aEvent) = 0;
	};






//
// ----> CHlpView (header)
//
class CHlpView : public CCoeControl, public MCoeControlObserver, public MHlpModelObserver, public MZoomableControl
    {
//
public: // ZOOM ENUMERATION
//
	enum TZoomStates
		{
		EZoomStateOne		= 750,
		EZoomStateTwo		= TZoomFactor::EZoomOneToOne,
		EZoomStateThree		= 1250
		};

//
public: // CONSTRUCT / DESTRUCT
//
	CHlpView(CHlpAppUi& aAppUi, CHlpModel& aModel);
	~CHlpView();
	void ConstructL();

//
public:	// NEW
//
	void PopulateCategoryListL();
	void PopulateTopicListL(const TDesC& aTopic);
	void UpdateEdwinL(const CHlpItem& aItem);

//
public: // FROM MZoomableControl
//
	void HandleZoomEventL(TZoomEvent aEvent);

//
public:	// FROM CCoeControl
//
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

//
private: // FROM MCoeControlObserver
//
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

//
private: // FROM MHlpModelObserver
//
	void HandleModelEventL(TInt aEvent);

//
private: // FROM MPictureFactory
//
	void NewPictureL(TPictureHeader& aHdr,const CStreamStore& aDeferredPictureStore) const;

//
private: // FROM MRichTextStoreResolver
//
	const CStreamStore& StreamStoreL(TInt aPos) const;

//
private: // NEW INTERNAL
//
	static TInt ZoomForIndex(TInt aIndex);

//
private: // MEMBER DATA
//
	CCoeControl* iFocusedControl;

	// Referenced
	CHlpAppUi& iAppUi;
	CHlpModel& iHelpModel;
	
	// Owned
	TInt iLastResponseFromHelpModel;
	TInt iCurrentZoom;
	TZoomFactor iZoom;
	CEikChoiceList* iHelpCategories;
	CEikChoiceList* iHelpTopicsForCategory;
	CEikRichTextEditor* iRichTextEditor;

	CHlpList* iCurrentTopics;
	CHlpTopic* iLastTopic;
    };









//
// ----> CHlpMainView (header)
//
class CHlpMainView : public CCoeControl, public MCoeControlObserver, public MHlpModelObserver
    {
//
public: // CONSTRUCT / DESTRUCT
//
	CHlpMainView(CHlpAppUi& aAppUi, CHlpModel& aModel);
	~CHlpMainView();
	void ConstructL();

//
public: // ACCESS
//
	TBool IsSearchPromptVisible() const;
	TBool IsSearchResultVisible() const;

	void ShowSearchPrompt(TBool aShow);
	void MakeResultsVisible(TBool aVisible);

//
public:	// FROM CCoeControl
//
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

//
public:	// FROM MCoeControlObserver
//
	void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

//
public:	// FROM MHlpModelObserver
//
	void HandleModelEventL(TInt aEvent);

//
public: // INTERNAL
//
	void DoSearchL();
	
//
private: // MEMBER DATA
//
	// Referenced
	CHlpAppUi& iAppUi;
	CHlpModel& iModel;

	// Owned
	TInt iLastHelpModelResponse;
	CHlpView* iView;
	CHlpListBox* iSearchResults;
	CHlpSearchPrompt* iSearchPrompt;
	};









//
// ----> CHlpAppUi (header)
//
class CHlpDocument : public CEikDocument
	{
//
public:
//
	CHlpDocument(CEikApplication& aApp);
	~CHlpDocument();

	void NewDocumentL();
	inline CHlpModel& Model() const { return *iModel; }
	inline CHlpAppUi& AppUi() const;

//
private: // FROM CEikDocument
//
	CEikAppUi* CreateAppUiL();

//
private: // MEMBER DATA
//
	CHlpModel* iModel;
	};








//
// ----> CHlpApplication (header)
//
class CHlpApplication : public CEikApplication
	{
//
private: // FROM CEikApplication - pure virutal
//
	CApaDocument* CreateDocumentL();
	// FROM CApaApplication - pure virutal	
	TUid AppDllUid() const;
	};








//
// ----> CHlpAppUi (header)
//
class CHlpAppUi : public CEikAppUi
    {
//
public:
//
	~CHlpAppUi();
    void ConstructL();

	inline CHlpDocument& Document() const	{ return *STATIC_CAST(CHlpDocument*, iDocument); }
	inline CHlpMainView& View() const		{ return *iView; }
	inline void SetZoomHandler(MZoomableControl* aZoomableControl) { iZoomHandler = aZoomableControl; }

//
private: // FROM CEikAppUi
//
	void HandleCommandL(TInt aCommand);
	void DynInitMenuPaneL(TInt aMenuId, CEikMenuPane* aMenuPane);

//
private: // NEW
//
	void HandleZoomL(TInt aZoomEvent);

//
private: // MEMBER DATA
//
	CHlpMainView* iView;
	MZoomableControl* iZoomHandler;
	};




inline CHlpAppUi& CHlpDocument::AppUi() const
	{
	return *STATIC_CAST(CHlpAppUi*, iAppUi); 
	}


#endif
