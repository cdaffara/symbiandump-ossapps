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
* Description:   
*
*/



#ifndef PROFILER_GUI_APP_H
#define PROFILER_GUI_APP_H


// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidProfilerGui = { 0x2001E5AE };

// CLASS DECLARATION

/**
* CProfilerGuiApp application class.
* Provides factory to create concrete document object.
* 
*/
class CProfilerGuiApp : public CAknApplication
    {
    
    public: // Functions from base classes

    private:

        /**
        * From CApaApplication, creates CProfilerGuiDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidProfilerGui).
        * @return The value of KUidProfilerGui.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

