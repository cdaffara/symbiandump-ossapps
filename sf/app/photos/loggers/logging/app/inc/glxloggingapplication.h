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



#ifndef __GLXLOGGING_APPLICATION_H__
#define __GLXLOGGING_APPLICATION_H__

#include <aknapp.h>


/** 
  @class CGlxLoggingApplication
  
  @discussion An instance of CGlxLoggingApplication is the application part of the AVKON
  application framework for the GlxLogging example application
  */
NONSHARABLE_CLASS( CGlxLoggingApplication ) : public CAknApplication
    {
    public:  // from CAknApplication

    /** 
      @function AppDllUid
      
      @discussion Returns the application DLL UID value
      @result the UID of this Application/Dll
      */
    TUid AppDllUid() const;

    protected: // from CAknApplication
    /** 
      @function CreateDocumentL
      
      @discussion Create a CApaDocument object and return a pointer to it
      @result a pointer to the created document
      */
    CApaDocument* CreateDocumentL();
    };

#endif // __GLXLOGGING_APPLICATION_H__
