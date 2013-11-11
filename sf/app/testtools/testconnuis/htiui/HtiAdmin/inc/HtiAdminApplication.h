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
* Description:  Application class
*
*/


#ifndef __HTIADMIN_APPLICATION_H__
#define __HTIADMIN_APPLICATION_H__

// INCLUDES

#include <aknapp.h>

// CLASS DECLARATION
/**
    An instance of CHtiAdminApplication is the application part of the AVKON
  application framework for the HtiAdmin example application
  */
class CHtiAdminApplication : public CAknApplication
    {
public:  // from CAknApplication

/**
    Returns the application DLL UID value
  @return the UID of this Application/Dll
  */
    TUid AppDllUid() const;

protected: // from CAknApplication
/**
    Create a CApaDocument object and return a pointer to it
  @return a pointer to the created document
  */
    CApaDocument* CreateDocumentL();
    };

#endif // __HTIADMIN_APPLICATION_H__


// End of File
