/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging utility application for MC Photos
*
*/



#ifndef __GLXLOGGING_DOCUMENT_H__
#define __GLXLOGGING_DOCUMENT_H__


#include <AknDoc.h>

// Forward references
class CGlxLoggingAppUi;
class CEikApplication;


/** 
  @class CGlxLoggingDocument
  
  @discussion An instance of class CGlxLoggingDocument is the Document part of the AVKON
  application framework for the GlxLogging application
  */
NONSHARABLE_CLASS( CGlxLoggingDocument ) : public CAknDocument
    {
public:

    /**
        @function NewL
        @param aApp application creating this document
        @result a pointer to the created instance of CGlxLoggingDocument
    */
    static CGlxLoggingDocument* NewL(CEikApplication& aApp);

    /**
      @function NewLC

      @param aApp application creating this document
      @result a pointer to the created instance of CGlxLoggingDocument
      */
    static CGlxLoggingDocument* NewLC(CEikApplication& aApp);

    /**
      @function ~CGlxLoggingDocument
      */
    ~CGlxLoggingDocument();

public: // from CAknDocument
    /**
      @function CreateAppUiL 
      @result a pointer to the created instance of the AppUi created
      */
    CEikAppUi* CreateAppUiL();

private:

    /**
      @function ConstructL
      */
    void ConstructL();

    /**
      @function CGlxLoggingDocument
      @param aApp application creating this document
      */
    CGlxLoggingDocument(CEikApplication& aApp);

    };


#endif // __GLXLOGGING_DOCUMENT_H__
