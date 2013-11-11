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
#include "clog.h"
#include "directprintappappui.h"
#include "directprintappdocument.h"
#include "directprintmodel.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDirectPrintAppDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDirectPrintAppDocument* CDirectPrintAppDocument::NewL(CEikApplication& aApp)
	{
	CDirectPrintAppDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDirectPrintAppDocument* CDirectPrintAppDocument::NewLC(CEikApplication& aApp)
	{
	CDirectPrintAppDocument* self = new (ELeave) CDirectPrintAppDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDirectPrintAppDocument::ConstructL()
	{
	LOG("CDirectPrintAppDocument::ConstructL BEGIN");
	// create the model object
	iModel = CDirectPrintModel::NewL();
	LOG("CDirectPrintAppDocument::ConstructL END");
	}

// -----------------------------------------------------------------------------
// CDirectPrintAppDocument::CDirectPrintAppDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDirectPrintAppDocument::CDirectPrintAppDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CDirectPrintAppDocument::~CDirectPrintAppDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CDirectPrintAppDocument::~CDirectPrintAppDocument()
	{
	delete iModel;
	}

// ---------------------------------------------------------------------------
// CDirectPrintAppDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CDirectPrintAppDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CDirectPrintAppAppUi;
	}

CDirectPrintModel& CDirectPrintAppDocument::Model()
	{
	return *iModel;
	}

// End of File
