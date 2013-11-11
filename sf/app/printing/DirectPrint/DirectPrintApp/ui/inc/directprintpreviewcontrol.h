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
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef __DIRECTPRINTPREVIEWCONTROL_H__
#define __DIRECTPRINTPREVIEWCONTROL_H__

// System includes
#include <eikbctrl.h>
#include <prnprev.h>

// Forward declarations
class CPrintSetup;
class TPrintPreviewFormat;
class MPageRegionPrinter;
class MPrintProcessObserver;

// Class declaration
/**
 *  Preview control class
 *  more_complete_description
 */
class CDirectPrintPreviewControl : public CEikBorderedControl
								 , public MCoeControlObserver
	{
public:
	// Data types
	/** The index of zoom level */
	enum TPreviewZoomLevel
		{
		EPreviewZoomLevel1, // display into view area
		EPreviewZoomLevel2,
		EPreviewZoomLevel3,
		EPreviewZoomLevel4,
		EPreviewZoomLevel5,
		EPreviewZoomLevelMax
		};

public:
	/** Constructors */
	static CDirectPrintPreviewControl* NewL(const CCoeControl* aParent);
	static CDirectPrintPreviewControl* NewLC(const CCoeControl* aParent);
	/** Destructor */
	virtual ~CDirectPrintPreviewControl();

public:
	/**
	 * Set observer.
	 *
	 * @param aObserver Pointer to observer.
	 */
	void SetPreviewImageObserver(MPrintProcessObserver* aObserver);

	/**
	 * Initialize display format.
	 *
	 * @param aPrintSetup Setup information.
	 * @param aBodyPrinter Body printer.
	 * @param aFormat Display format information.
	 * @param aNumPagesInDoc Number of all pages.
	 * @param aFirstPageToDisplay Page number of first display.
	 * @param aNumPagesToView Number of pages to view.
	 * @param aMarginState Margin information.
	 */
	void InitializeFormat(	CPrintSetup& aPrintSetup,
							MPageRegionPrinter& aBodyPrinter,
							TPrintPreviewFormat& aFormat,
							TInt& aNumPagesInDoc,
							TInt& aFirstPageToDisplay,
							TInt aNumPagesToView,
							CPrintPreviewImage::TMarginState& aMarginState);
	/**
	 * Set display format.
	 *
	 * @param aFirstPageToDisplay Page number of first display.
	 * @param aNumPagesToView Count of pages to view.
	 */
	void SetFormatAndDrawL(TInt& aFirstPageToDisplay,TInt& aNumPagesToView);
	/**
	 * Set margin.
	 *
	 * @param aMarginState Margin information.
	 */
	void SetMarginAndDrawL(CPrintPreviewImage::TMarginState aMarginState);
	/**
	 * Set number of bands.
	 *
	 * @param aNumBands Number of bands.
	 */
	void SetNumBands(TInt aNumBands);
	/**
	 * Set page number.
	 *
	 * @param aNumPagesInDoc Number of all pages.
	 * @param aFirstPageToDisplay Page number of first display.
	 * @param aNumPagesToView Number of pages to view.
	 */
	void SetNumPagesInDoc(TInt aNumPagesInDoc,TInt aFirstPageToDisplay,TInt aNumPagesToView);

public: // from CCoeControl
	/**
	 * From CCoeControl.
	 * Activate.
	 */
	void ActivateL();
	/**
	 * From CCoeControl.
	 * Draw display.
	 *
	 * @param aRect Rect to draw.
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * From CCoeControl.
	 * Handles key events.
	 *
	 * @param aKeyEvent The code of key event.
	 * @param aType The code of event.
	 * @return The code of key response.
	 */
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	/**
	 * From CCoeControl.
	 * Constructor.
	 *
	 * @param aReader ResourceReader.
	 */
//	void ConstructFromResourceL(TResourceReader& aReader);
	/**
	 * Set flag.
	 *
	 * @param aDraw Draw flag.
	 */
	void SetDrawFlag(TBool aDraw);
	/**
	 * Get flag.
	 *
	 * @return Draw flag.
	 */
	TBool DrawFlag();
	/**
	 * From CCoeControl.
	 * Get input capabilities.
	 *
	 * @return Input capabilities.
	 */
	TCoeInputCapabilities InputCapabilities() const;

	// for zoom
	/**
	 * Set zoom level.
	 *
	 * @param aZoomLevel Zoom level.
	 */
	void SetZoomLevel(TInt aZoomLevel);
	/**
	 * Get zoom level.
	 *
	 * @return Zoom level.
	 */
	TInt GetZoomLebel();
	/**
	 * Increment zoom level.
	 */
	void IncrementZoomLevel();
	/**
	 * Decrement zoom level.
	 */
	void DecrementZoomLevel();

protected: // from CCoeControl
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;

	void WriteInternalStateL(RWriteStream& aWriteStream) const;

	/**
	 * From CoeControl, HandlePointerEventL.
	 * Called by framework when a pointer touch event occurs.
	 * Note: although this method is compatible with earlier SDKs,
	 * it will not be called in SDKs without Touch support.
	 * @param aPointerEvent the information about this event
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private: // from CCoeControl
//	void SetContainerWindowL(const CCoeControl& aParent);

private: // from MCoeControlObserver
	void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEvent);

private:
	CDirectPrintPreviewControl();
	void ConstructL(const CCoeControl* aParent);

private:
	void CreateScrollButtonsL();
	void DrawPreview(const TRect& aRect);
	void SwitchFirstPageToDisplayL(TInt& aNewFirstPageToDisplay);
	void DrawButtons() const;
	void SetZoomLevelAndDispalyRect(TInt aZoomLevel);
	void CreatePreviewImageL();

private:
	/** Pointer to button controls. */
//	CEikScrollButton* iButton[2];
	/** Pointer to image. */
	CPrintPreviewImage* iPreview;
	/** Pointer to setup information. */
	CPrintSetup* iPrintSetup;
	/** Pointer to body printer. */
	MPageRegionPrinter* iBodyPrinter;
	/** Pointer to format information. */
	TPrintPreviewFormat* iPrintPreviewFormat;
	/** Pointer to page number. */
	TInt* iFirstPageToDisplay;
	/** Number of all pages. */
	TInt iNumPagesInDoc;
	/** Number of pages to view. */
	TInt iNumPagesToView;
	/** Number of bands. */
	TInt iNumBands;
	/** Margin information. */
	CPrintPreviewImage::TMarginState iMarginState;
	/** Display rect. */
	TRect iDisplayRect;
	/** Draw flag. */
	TBool iPreviewDraw;
	/** Display point. */
	TPoint iDisplayPoint;
	/** Position of pointer. */
	TPoint iPointerDownPos;
	/** Zoom level. */
	TInt  iZoomLevel;			// set TPreviewZoomLevel
	};

#endif // __DIRECTPRINTPREVIEWCONTROL_H__
