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
* Description: This file contains CAppUIApp class declaration.
*
*/

#ifndef APPUIAPP_H
#define APPUIAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidAppUI = { 0x1028311D };

// CLASS DECLARATION

/**
* CAppUIApp application class.
* Provides factory to create concrete document object.
* 
*/
class CAppUIApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CAppUIDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidAppUI).
        * @return The value of KUidAppUI.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File
