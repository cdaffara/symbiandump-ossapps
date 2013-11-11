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


#ifndef PERFMON_APP_H
#define PERFMON_APP_H


// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
const TUid KUidPerfMon = { 0x20011385 };

// CLASS DECLARATION

/**
* CPerfMonApp application class.
* Provides factory to create concrete document object.
* 
*/
class CPerfMonApp : public CAknApplication
    {
    
    public: // Functions from base classes
        /**
        * From CApaApplication, overridden to enable INI file support.
        * @return A pointer to the dictionary store
        */
    CDictionaryStore* OpenIniFileLC(RFs& aFs) const;
    private:

        /**
        * From CApaApplication, creates CPerfMonDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidPerfMon).
        * @return The value of KUidPerfMon.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

