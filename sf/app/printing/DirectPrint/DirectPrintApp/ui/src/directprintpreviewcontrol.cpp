/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
*
*/

// INCLUDE FILES
#include <coemain.h>
#include <barsread.h>
#include <DirectPrintApp.rsg>
#include <prninf.h>
#include <prnsetup.h>
#include <gdi.h>
#include <prnprev.h>
#include <eikenv.h>
#include <gulbordr.h>					// TGulBorder
#include <e32cmn.h>

#include "directprintapp.pan"
#include "directprintapp.hrh"
#include "directprintpreviewcontrol.h"

const TInt KFirstPage=0;

const TInt KDraggingAccepted = 10;

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDirectPrintPreviewControl* CDirectPrintPreviewControl::NewL(const CCoeControl* aParent)
	{
	CDirectPrintPreviewControl* self = CDirectPrintPreviewControl::NewLC(aParent);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDirectPrintPreviewControl* CDirectPrintPreviewControl::NewLC(const CCoeControl* aParent)
	{
	CDirectPrintPreviewControl* self = new(ELeave) CDirectPrintPreviewControl();
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	return self;
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::CDirectPrintPreviewControl()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectPrintPreviewControl::CDirectPrintPreviewControl()
	{
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::~CDirectPrintPreviewControl()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectPrintPreviewControl::~CDirectPrintPreviewControl()
	{
	Window().SetPointerGrab(EFalse);
	delete iPreview;
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDirectPrintPreviewControl::ConstructL(const CCoeControl* aParent)
	{
	// Create a window for this application view
	if (aParent)
		{
		CreateBackedUpWindowL(*aParent->DrawableWindow(), EGray4);
		}
	else
		{
		CreateBackedUpWindowL((RWindowTreeNode&)iCoeEnv->RootWin(), EGray4);
		}

	EnableDragEvents();
	Window().SetPointerGrab(ETrue);
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::SetPreviewImageObserver()
//
// -----------------------------------------------------------------------------
//
void CDirectPrintPreviewControl::SetPreviewImageObserver(MPrintProcessObserver* aObserver)
	{
	__ASSERT_DEBUG(iPreview,Panic(EDirectPrintNullPointer));
	iPreview->SetObserver(*aObserver);
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::CountComponentControls()
// Called by framework
// -----------------------------------------------------------------------------
//
TInt CDirectPrintPreviewControl::CountComponentControls() const
	{
//	return(2);
	return(0);
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::ComponentControl()
// Called by framework
// -----------------------------------------------------------------------------
//
CCoeControl* CDirectPrintPreviewControl::ComponentControl( TInt aIndex ) const
	{
	CCoeControl* control=NULL;
	switch (aIndex)
		{
	case 0:
//		control=iButton[0];
		break;
	case 1:
//		control=iButton[1];
		break;
		}
	return(control);
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::WriteInternalStateL()
//
// -----------------------------------------------------------------------------
//
#ifndef _DEBUG
void CDirectPrintPreviewControl::WriteInternalStateL(RWriteStream& /*aWriteStream*/) const
	{
	}
#else
void CDirectPrintPreviewControl::WriteInternalStateL(RWriteStream& aWriteStream) const
	{
	CEikBorderedControl::WriteInternalStateL(aWriteStream);
	}
#endif

// -----------------------------------------------------------------------------
// CDirectPrintAppAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs,
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CDirectPrintPreviewControl::HandlePointerEventL(
		const TPointerEvent& aPointerEvent)
	{
	const TRect rect=Rect();
	if( !rect.Contains(aPointerEvent.iPosition) )
		{
		return;
		}

	switch ( aPointerEvent.iType )
		{
		case TPointerEvent::EButton1Down:
			{
			iPointerDownPos = aPointerEvent.iPosition;
			}
			break;
		case TPointerEvent::EButton1Up:
		case TPointerEvent::EDrag:
			{
			TInt x =  aPointerEvent.iPosition.iX - iPointerDownPos.iX;
			TInt y = aPointerEvent.iPosition.iY - iPointerDownPos.iY;
			if (Abs(x) > KDraggingAccepted || Abs(y) > KDraggingAccepted)
				{
				TPoint newPos;
				TInt maxWidth = iDisplayRect.Width() - rect.Width();
				TInt maxHeight = iDisplayRect.Height() - rect.Height();
				newPos.iX = Min(iDisplayPoint.iX + x, maxWidth);
				if (newPos.iX <= (maxWidth*-1))
					newPos.iX = (maxWidth*-1);
				newPos.iY = Min(iDisplayPoint.iY + y, maxHeight);
				if (newPos.iY <= (maxHeight*-1))
					newPos.iY = (maxHeight*-1);
				iDisplayPoint = newPos;
				iPointerDownPos = aPointerEvent.iPosition;
				DrawNow();
				}
			}
			break;
		default:
			break;
		}
	}

void CDirectPrintPreviewControl::ActivateL()
	{
	DrawButtons();
	CCoeControl::ActivateL();
	}

TCoeInputCapabilities CDirectPrintPreviewControl::InputCapabilities() const
	{
	return TCoeInputCapabilities(TCoeInputCapabilities::ENavigation);
	}

void CDirectPrintPreviewControl::InitializeFormat(
										CPrintSetup& aPrintSetup,
										MPageRegionPrinter& aBodyPrinter,
										TPrintPreviewFormat& aFormat,
										TInt& aNumPagesInDoc,
										TInt& aFirstPageToDisplay,
										TInt aNumPagesToView,
										CPrintPreviewImage::TMarginState& aMarginState)
	{
	iPrintSetup=&aPrintSetup;
	iBodyPrinter=&aBodyPrinter;
	iPrintPreviewFormat=&aFormat;
	iFirstPageToDisplay=&aFirstPageToDisplay;
	iNumPagesInDoc=aNumPagesInDoc;
	iNumPagesToView=aNumPagesToView;
	iMarginState=aMarginState;
	}

void CDirectPrintPreviewControl::SetNumBands(TInt aNumBands)
	{
	iNumBands=aNumBands;
	if (iPreview)
		{
		iPreview->SetNumBands(aNumBands);
		}
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CDirectPrintPreviewControl::SizeChanged()
	// can't recover if CPrintPreviewImage::NewL() fails !!
	{
	iPreviewDraw=ETrue;

	// set zoom level 
	SetZoomLevelAndDispalyRect(EPreviewZoomLevel1);

	// create CPrintPreviewImage object
	TRAP_IGNORE(CreatePreviewImageL());
	}

// -----------------------------------------------------------------------------
// CDirectPrintPreviewControl::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CDirectPrintPreviewControl::Draw(const TRect& aRect) const
	{
	if (!iPreviewDraw)
		return;
//	DrawButtons();
	CWindowGc& gc=SystemGc();
	iBorder.Draw(gc,Rect());
	gc.Clear();
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushColor(KRgbWhite);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
//	TRect rectAboveButton(TPoint(iDisplayRect.iTl.iX-KScrollButtonWidth,iDisplayRect.iTl.iY),TSize(KScrollButtonWidth,iDisplayRect.Height()-(iButton[0]->IsVisible()? KScrollButtonWidth : 0)));
//	gc.DrawRect(rectAboveButton);
//	rectAboveButton=(TPoint(iDisplayRect.iTl.iX-KScrollButtonWidth,iDisplayRect.iTl.iY),TSize(KScrollButtonWidth,iDisplayRect.Height()-(iButton[1]->IsVisible()? KScrollButtonWidth : 0)));
//	rectAboveButton.Move(iDisplayRect.Width()+KScrollButtonWidth+1,1); // why are the 1s needed???
//	gc.DrawRect(rectAboveButton);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	CONST_CAST(CDirectPrintPreviewControl*,this)->DrawPreview(aRect);  // cast away constness
	}

void CDirectPrintPreviewControl::SetDrawFlag(TBool aDraw)
	{
	iPreviewDraw=aDraw;
	}

TBool CDirectPrintPreviewControl::DrawFlag()
	{
	return iPreviewDraw;
	}

void CDirectPrintPreviewControl::SetFormatAndDrawL(TInt& aFirstPageToDisplay,TInt& aNumPagesToView)
	{
	iPreview->FormatL(iNumPagesInDoc,aFirstPageToDisplay,aNumPagesToView);
	*iFirstPageToDisplay=aFirstPageToDisplay;
	iNumPagesToView=aNumPagesToView;
	DrawButtons();
	DrawNow();
	}

void CDirectPrintPreviewControl::SetMarginAndDrawL(CPrintPreviewImage::TMarginState aMarginState)
	{
	iPreview->SetMargins(aMarginState);
	iMarginState=aMarginState;
	DrawNow();
	}

void CDirectPrintPreviewControl::SetNumPagesInDoc(TInt aNumPagesInDoc,TInt aFirstPageToDisplay,TInt aNumPagesToView)
	{
	iNumPagesInDoc=aNumPagesInDoc;
	*iFirstPageToDisplay=aFirstPageToDisplay;
	iNumPagesToView=aNumPagesToView;
	}

void CDirectPrintPreviewControl::DrawPreview(const TRect& aRect)
	{
	TRAPD(err,iPreview->DrawL(iDisplayPoint,aRect));
	if (err)
		{
		SystemGc().Clear(iDisplayRect);
		CEikonEnv::Static()->NotifyIdleErrorWhileRedrawing(err);
		}
	}

TKeyResponse CDirectPrintPreviewControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
	{
	TInt newFirstPageToDisplay=*iFirstPageToDisplay;
	switch (aKeyEvent.iCode)
		{
	case EKeyLeftArrow:
	case EKeyPageUp:
		newFirstPageToDisplay--;
		break;
	case EKeyRightArrow:
	case EKeyPageDown:
		newFirstPageToDisplay++;
		break;
	case EKeyHome:
		newFirstPageToDisplay=KFirstPage;
		break;
	case EKeyEnd:
		newFirstPageToDisplay=iNumPagesInDoc-iNumPagesToView;
		break;
	default:
		return(EKeyWasNotConsumed);

		}
	SwitchFirstPageToDisplayL(newFirstPageToDisplay);
	return(EKeyWasConsumed);
	}

void CDirectPrintPreviewControl::SwitchFirstPageToDisplayL(TInt& aNewFirstPageToDisplay)
	{
	if (aNewFirstPageToDisplay<KFirstPage)
		aNewFirstPageToDisplay=KFirstPage;
	if (aNewFirstPageToDisplay>iNumPagesInDoc-iNumPagesToView)
		aNewFirstPageToDisplay=iNumPagesInDoc-iNumPagesToView;
	if (aNewFirstPageToDisplay!=*iFirstPageToDisplay)
		SetFormatAndDrawL(aNewFirstPageToDisplay,iNumPagesToView);
	}

void CDirectPrintPreviewControl::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent aEventType)
	{
	if (aEventType!=EEventStateChanged)
		return;
	TInt newFirstPageToDisplay=*iFirstPageToDisplay;
//	if (aControl==iButton[0])
//		newFirstPageToDisplay--;
//	else if (aControl==iButton[1])
//		newFirstPageToDisplay++;
	SwitchFirstPageToDisplayL(newFirstPageToDisplay);
	}

void CDirectPrintPreviewControl::DrawButtons() const
	{
	if (iNumPagesToView==iNumPagesInDoc)
		{
//		iButton[1]->MakeVisible(EFalse);
//		iButton[0]->MakeVisible(EFalse);
		return;
		}
//	TBool leftvisible=ETrue;
//	TBool rightvisible=ETrue;
//	if (*iFirstPageToDisplay==KFirstPage)
//		leftvisible=EFalse;
//	else
//		{
//		if (*iFirstPageToDisplay>=iNumPagesInDoc-iNumPagesToView)
//			rightvisible=EFalse;
//		}
//	iButton[0]->MakeVisible(leftvisible);
//	iButton[1]->MakeVisible(rightvisible);
////	iButton[0]->DrawNow();
////	iButton[1]->DrawNow();
	}


// for zoom
void CDirectPrintPreviewControl::SetZoomLevel(TInt /*aZoomLevel*/)
	{
	SetZoomLevelAndDispalyRect(iZoomLevel);
	}

TInt CDirectPrintPreviewControl::GetZoomLebel()
	{
	return (TInt)iZoomLevel;
	}

void CDirectPrintPreviewControl::IncrementZoomLevel()
	{
	TInt zoom = iZoomLevel + 1;
	if (zoom >= EPreviewZoomLevel1
	 && zoom < EPreviewZoomLevelMax)
		{
		SetZoomLevelAndDispalyRect(zoom);
		TRAPD(err, CreatePreviewImageL());
		if (err == KErrNone)
			{
			DrawNow();
			}
		}
	}

void CDirectPrintPreviewControl::DecrementZoomLevel()
	{
	TInt zoom = iZoomLevel - 1;
	if (zoom >= EPreviewZoomLevel1)
		{
		SetZoomLevelAndDispalyRect(zoom);
		TRAPD(err, CreatePreviewImageL());
		if (err == KErrNone)
			{
			DrawNow();
			}
		}
	}

	// set zoom level 
void CDirectPrintPreviewControl::SetZoomLevelAndDispalyRect(TInt aZoomLevel)
	{
	static const TInt KZoomPercent[EPreviewZoomLevelMax] = {100, 120, 140, 160, 180};
	TInt oldZoomLevel = iZoomLevel;
	if (aZoomLevel == EPreviewZoomLevel1)
		{
		iDisplayRect = iBorder.InnerRect(Rect());
		iDisplayPoint = iDisplayRect.iTl;
		}
	else
		{
		TRect rect(iBorder.InnerRect(Rect()));
		TSize newSize;
		newSize.iWidth = (rect.Size().iWidth * KZoomPercent[aZoomLevel] + 5) / KZoomPercent[EPreviewZoomLevel1];
		newSize.iHeight = (rect.Size().iHeight * KZoomPercent[aZoomLevel] + 5) / KZoomPercent[EPreviewZoomLevel1];
		iDisplayRect.SetSize(newSize);
		if (oldZoomLevel != EPreviewZoomLevel1)
			{
			TPoint newPos;
			newPos.iX = Max(0, ((iDisplayPoint.iX  * KZoomPercent[aZoomLevel] + 5) / KZoomPercent[EPreviewZoomLevel1]));
			newPos.iY = Max(0, ((iDisplayPoint.iY  * KZoomPercent[aZoomLevel] + 5) / KZoomPercent[EPreviewZoomLevel1]));
			iDisplayPoint = newPos;
			}
		}
	iZoomLevel = aZoomLevel;
	}

void CDirectPrintPreviewControl::CreatePreviewImageL()
	{
	if (iPreview)
		{
		delete iPreview;
		iPreview=NULL;
		}

	iPreview = CPrintPreviewImage::NewL(*iPrintSetup,
										*iBodyPrinter,
										CEikonEnv::Static()->WsSession(),
										Window(),
										iDisplayRect.Size(),
										*iPrintPreviewFormat);
	iPreview->SetNumBands(iNumBands);
	iPreview->FormatL(iNumPagesInDoc,*iFirstPageToDisplay,iNumPagesToView);
	iPreview->SetMargins(iMarginState);
//	TRect buttonRect(TPoint(iDisplayRect.iTl.iX-KScrollButtonWidth,iDisplayRect.iBr.iY-KScrollButtonWidth),
//					TSize(KScrollButtonWidth,KScrollButtonWidth));
//	iButton[0]->SetRect(buttonRect);
//	buttonRect.Move(iDisplayRect.Width()+KScrollButtonWidth,0);
//	iButton[1]->SetRect(buttonRect);
	}

// End of file
