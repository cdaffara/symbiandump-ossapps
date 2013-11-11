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


#ifndef CTEMPLATESETTINGS_H
#define CTEMPLATESETTINGS_H

#include <e32std.h>
#include <e32base.h>

class CTemplateData;
class CRealFactory;

/**
 *
 * Encapsulates the complete set of templates
 *
 */  
class CTemplateSettings
    : public CBase
    {
    public:     // Constuctors and destructors

        /**
         *  Two-phase constructor
         *
         *  @return Initialized instance of itself
         */
        static CTemplateSettings* NewL( CRealFactory* afactory );
        static CTemplateSettings* NewLC( CRealFactory* afactory );
        
        /**
         *  Destructor
         */
        virtual ~CTemplateSettings();

    protected:  // Constuctors and destructors

        /**
         *  Constructor
         */
        CTemplateSettings( CRealFactory* afactory );

    public:     // New methods

        /**
         *  Clones current template settings
         *
         *  @return cloned instance
         */
        CTemplateSettings* CloneL();

        /**
         *  Sets new active template, leaves on error
         *
         *  @param aUid Uid of the new active template
         */
        void SetActiveTemplateL( TInt aUid );

    public:     // Data

        // Set of templates
        RPointerArray<CTemplateData> iTemplates;

        // Currently active template
        TUint iActiveTemplateUid;
        
        //Pointer to interface factory
        CRealFactory* iFactory;
    };

#endif //  CTEMPLATESETTINGS_H

//  End of File
