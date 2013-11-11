/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The application class for ConnTest
*
*/

#ifndef CONNTESTAPP_H
#define CONNTESTAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidConnTest = { 0x101F6D2B };

// CLASS DECLARATION

/**
* CConnTestApp application class.
* Provides factory to create concrete document object.
* 
*/
class CConnTestApp : public CAknApplication
    {
    
    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CConnTestDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidConnTest).
        * @return The value of KUidConnTest.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

