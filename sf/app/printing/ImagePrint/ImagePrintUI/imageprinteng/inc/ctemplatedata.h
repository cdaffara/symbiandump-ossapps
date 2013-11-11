/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CTEMPLATEDATA_H
#define CTEMPLATEDATA_H

#include <e32std.h>
#include <fbs.h>

/**
 *
 * Encapsulates the data of a single template
 *
 */
class CTemplateData 
    : public CBase
    {
    public:     // Constructors and destructors

        /**
         *  Standard two-phase constructor
         */
        static CTemplateData* NewLC();
        
        /**
         *  Standard two-phase constructor
         */
        static CTemplateData* NewL();

        /**
         *  Destructor
         */
        virtual ~CTemplateData();

    protected:  // Constructors and destructors

        /**
         *  Constructor
         */
        CTemplateData();

    public:     // New methods

        /**
         *  Clones the template data and returns it
         *
         *  @return cloned instance of the template data class instance
         */
        CTemplateData* CloneL();

    public:     // Data

        // Uid for the template, passed to the
        // engine, when the template is activated on
        // print preview
        TInt iUid;
    };

#endif //  CTEMPLATEDATA_H

//  End of File
