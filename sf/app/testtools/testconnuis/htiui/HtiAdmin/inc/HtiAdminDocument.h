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
* Description:  Document class
*
*/


#ifndef __HTIADMIN_DOCUMENT_H__
#define __HTIADMIN_DOCUMENT_H__

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CHtiAdminAppUi;
class CEikApplication;

// CLASS DECLARATION
/**
  An instance of class CHtiAdminDocument is the Document part of the AVKON
  application framework for the HtiAdmin example application
  */
class CHtiAdminDocument : public CAknDocument
    {
public:

/**
    Construct a CHtiAdminDocument for the AVKON application aApp
  using two phase construction, and return a pointer to the created object

  @param aApp application creating this document
  @return a pointer to the created instance of CHtiAdminDocument
  */
    static CHtiAdminDocument* NewL(CEikApplication& aApp);

/**
    Construct a CHtiAdminDocument for the AVKON application aApp
  using two phase construction, and return a pointer to the created object
  @param aApp application creating this document

  @return a pointer to the created instance of CHtiAdminDocument
  */
    static CHtiAdminDocument* NewLC(CEikApplication& aApp);

/**
Destroy the object and release all memory objects
  */
    ~CHtiAdminDocument();

public: // from CAknDocument
/**
    Create a CHtiAdminAppUi object and return a pointer to it
  @return a pointer to the created instance of the AppUi created
  */
    CEikAppUi* CreateAppUiL();

private:

/**
    Perform the second phase construction of a CHtiAdminDocument object
  */
    void ConstructL();

/**
    Perform the first phase of two phase construction
  @param aApp application creating this document
  */
    CHtiAdminDocument(CEikApplication& aApp);

    };


#endif // __HTIADMIN_DOCUMENT_H__


// End of File
