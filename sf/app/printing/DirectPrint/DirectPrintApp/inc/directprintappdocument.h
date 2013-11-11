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
#ifndef __DIRECTPRINTAPPDOCUMENT_h__
#define __DIRECTPRINTAPPDOCUMENT_h__

// System includes
#include <akndoc.h>

// Forward declarations
class CDirectPrintAppAppUi;
class CEikApplication;

// Class declaration
class CDirectPrintModel;

/**
 * CDirectPrintAppDocument application class.
 * An instance of class CDirectPrintAppDocument is the Document part of the
 * AVKON application framework for the DirectPrintApp example application.
 */
class CDirectPrintAppDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CDirectPrintAppDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CDirectPrintAppDocument.
	 */
	static CDirectPrintAppDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CDirectPrintAppDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CDirectPrintAppDocument.
	 */
	static CDirectPrintAppDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CDirectPrintAppDocument
	 * Virtual Destructor.
	 */
	virtual ~CDirectPrintAppDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CDirectPrintAppAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();
public:
	// New functions
	CDirectPrintModel& Model();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CDirectPrintAppDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CDirectPrintAppDocument(CEikApplication& aApp);


private:
	/** Model class */
	CDirectPrintModel*  iModel;


	};

#endif // __DIRECTPRINTAPPDOCUMENT_h__
// End of File
