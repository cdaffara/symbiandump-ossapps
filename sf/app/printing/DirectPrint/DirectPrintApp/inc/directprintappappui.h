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
#ifndef __DIRECTPRINTAPPAPPUI_H__
#define __DIRECTPRINTAPPAPPUI_H__

// System includes
#include <aknviewappui.h>
#include <aknnavide.h>
#include <AknTabObserver.h> 
#include <e32property.h>
#include <xmlengdocument.h>				// RXmlEngDocument
#include <xmlengdomparser.h>			// RXmlEngDOMParser
#include <xmlengdomimplementation.h>	// RXmlEngDOMImplementation

// User includes
#include "DirectPrintEngineObserver.h"
#include "DirectPrintBearerMngObserver.h"

// Forward declarations
class CDirectPrintMainView;
class CDirectPrintAppDocument;
class CDirectPrintEngine;
class CDirectPrintBearerMgr;
class CDirectPrintUiPluginMgr;

// Class declaration
class CDirectPrintAppAppUi : public CAknViewAppUi,
							 public MAknTabObserver,
							 public MDirectPrintEngineObserver,
							 public MDirectPrintBearerMngObserver
	{
public: // Constructors and destructor
	CDirectPrintAppAppUi();
	virtual ~CDirectPrintAppAppUi();
	void ConstructL();

private:
	// Functions from base classes

	/**
	 * From CEikAppUi, HandleCommandL.
	 * Takes care of command handling.
	 * @param aCommand Command to be handled.
	 */
	void HandleCommandL( TInt aCommand );

	/**
	 * From CEikAppUi, HandleResourceChangeL.
	 * Handles a change to the application's resources.
	 * @param aType Type of resources.
	 */
	void HandleResourceChangeL( TInt aType );

public: // from MAknTabObserver
	/**
	 * From MAknTabObserver, TabChangedL.
	 * Handles a change to the tabs.
	 * @param aIndex Index of tabs.
	 */
	void TabChangedL(TInt aIndex);

public: // from MDirectPrintEngineObserver
	/**
	 * From MDirectPrintEngineObserver, HandlePrintEventL.
	 * Handles user events.
	 * @param aEvent The ID of the event.
	 * @param aError The ID of the error.
	 * @param aParam Parameter.
	 */
	void HandlePrintEventL(TDirectPrintEngineEvents aEvent,
						   TInt aError, 
						   const TDesC& aParam);

public: // from MDirectPrintBearerMngObserver
	/**
	 * From MDirectPrintBearerMngObserver, HandleBearerMngEventL.
	 * Handles user events.
	 * @param aEvent The ID of the event.
	 * @param aError Error code.
	 * @param aParam Parameter.
	 */
	void HandleBearerMngEventL(TDirectPrintBearerMngEvents aEvent,
						   TInt aError, 
						   const TDesC& aParam);

	/**
	 * From MDirectPrintBearerMngObserver, PrintJobProgressEvent.
	 * Handles user events.
	 * @param aStatus The ID of the status.
	 * @param aPercentCompletion Percentage of progress.
	 * @param aJobStateCode The ID of job states.
	 */
	void PrintJobProgressEvent( TInt aStatus, 
							TInt aPercentCompletion, 
							TInt aJobStateCode );
							

	/**
	 * From MDirectPrintBearerMngObserver, PrintJobErrorEvent.
	 * Handles user events.
	 * @param aError The ID of the error.
	 * @param aErrorStringCode The ID of error strings.
	 */
	void PrintJobErrorEvent( TInt aError, 
							TInt aErrorStringCode );
							

	/**
	 * From MDirectPrintBearerMngObserver, PrinterStatusEvent.
	 * Handles user events.
	 * @param aError The ID of the error.
	 * @param aErrorStringCode The ID of error strings.
	 */
	void PrinterStatusEvent( TInt aError, 
							TInt aErrorStringCode );

	/**
	 * From MDirectPrintBearerMngObserver, PreviewImageEvent.
	 * Handles user events.
	 * @param aFsBitmapHandle The Handle of the bitmap.
	 */
	void PreviewImageEvent( TInt aFsBitmapHandle );

	/**
	 * From MDirectPrintBearerMngObserver, ShowMessageL.
	 * Show messages.
	 * @param aMsgLine1Code The ID of message.
	 * @param aMsgLine2Code The ID of message.
	 */
	void ShowMessageL( TInt aMsgLine1Code, 
							TInt aMsgLine2Code );

	/**
	 * From MDirectPrintBearerMngObserver, AskYesNoQuestionL.
	 * Show Yes/No question.
	 * @param aMsgLine1Code The ID of message.
	 * @param aMsgLine2Code The ID of message.
	 * @return ETrue is Yes. EFalse is No.
	 */
	TBool AskYesNoQuestionL( TInt aMsgLine1Code, 
							TInt aMsgLine2Code );

	/**
	 * From MDirectPrintBearerMngObserver, AskForInputL.
	 * input.
	 * @param aMsgLine1Code The ID of message.
	 * @param aMsgLine2Code The ID of message.
	 * @return Inputed strings.
	 */
	const TDesC& AskForInputL( TInt aMsgLine1Code, 
							TInt aMsgLine2Code );

public:
	/**
	 * engine object.
	 * 
	 * @return Pointer to a engine object.
	 */
	inline CDirectPrintEngine* Engine() {return iEngine;}

	/**
	 * document object.
	 * 
	 * @return Pointer to a document object.
	 */
	inline CDirectPrintAppDocument* Doc() {return iDoc;}

	/**
	 * Plugin manager object.
	 * 
	 * @return Reference to a plugin manager object.
	 */
	inline CDirectPrintUiPluginMgr& PluginMgr() {return *iPluginMgr;}

	/**
	 * Bearer manager object.
	 * 
	 * @return Pointer to a bearer manager object.
	 */
	inline CDirectPrintBearerMgr* BearerMgr() {return iBearerMgr;}

private:
	void MakeNaviPaneVisibleL(TBool aVisible);
	void MakeStatusPaneVisible(TBool aVisible);

	void InitializeViewsL();
	void LoadParamL();
	void GetParamFromAiwProviderL();

	void InitializeEngineL();
	void InitializeBearerMngL();
	void InitializePluginMgrL();

private: // Data
	// Used for navigation pane text
	CAknNavigationDecorator* iNaviDecorator;

	// Document class
	CDirectPrintAppDocument* iDoc;

	// previous view id
	TUid iBackViewUid;

	// For AIW calls;
	TInt iAiwState;

	// publish/subscribe of aiw print
	RProperty iAiwCall;

	// print engine
	CDirectPrintEngine* iEngine;
	
	// bearer manager
	CDirectPrintBearerMgr* iBearerMgr;

	// param xml object
	RXmlEngDocument iXmlEngDoc;
    RXmlEngDOMImplementation iXmlDomImpl;
    RXmlEngDOMParser iXmlParser;

	HBufC8*	iXmlParam;

	CDirectPrintUiPluginMgr* iPluginMgr;
	};

#endif // __DIRECTPRINTAPPAPPUI_H__

// End of File

