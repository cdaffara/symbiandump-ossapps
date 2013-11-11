/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*   See class description.
*
*/


#ifndef TPHONECMDPARAMPROGRESSNOTE_H
#define TPHONECMDPARAMPROGRESSNOTE_H

//  INCLUDES

#include <w32std.h>
#include <aknnotedialog.h> 
#include "tphonecommandparam.h"

// DATA TYPES

// CLASS DECLARATION

/**
*  A parameter class for note dialog information.
*/
class TPhoneCmdParamProgressNote : public TPhoneUICommandParam
    {   
    public:  
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TPhoneCmdParamProgressNote();

    public: 

        /**
        * Sets the resource ID for the note
        * @param aResourceId is the note resource ID
        */
        IMPORT_C void SetResourceId( TInt aResourceId );
        
         /**
        * Sets the wait time for the note
        * @param aWaitTime is the note wait time
        */
        IMPORT_C void SetWaitTime( TTimeIntervalMicroSeconds32 aTime );

        /**
        * Returns the note resource ID.
        * @return Returns the note resource ID.
        */
        IMPORT_C TInt ResourceId() const;
        
        /**
        * Returns the note wait time
        * @return Returns the note wait time.
        */
        IMPORT_C TTimeIntervalMicroSeconds32 WaitTime() const;

    private:    

        /**
        * Note resource ID
        */
        TInt iResourceId;
        
        /**
        * Note wait time
        */
        TTimeIntervalMicroSeconds32 iWaitTime;
    };

#endif // TPHONECMDPARAMPROGRESSNOTE_H
            
// End of File
