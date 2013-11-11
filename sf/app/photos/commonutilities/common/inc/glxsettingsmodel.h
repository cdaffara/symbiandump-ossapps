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
* Description:   MC Photos settings model
 *
*/




#ifndef _GLXSETTINGSMDL_H_
#define _GLXSETTINGSMDL_H_

#include <e32base.h>

// FORWARD DECLARATIONS

class CRepository;

// CONSTANTS
// LOCAL CONSTANTS NAMESPACE
namespace
	{
	// CenRep Key Ids
    const TUint32 KGlxSortOrderTagManager 	= 0x00000001;
    const TUint32 KGlxSortOrderTagBrowser 	= 0x00000002;
    const TUint32 KGlxIconHiddenState       = 0x00000003;
    const TUint32 KGlxDefaultOrientation    = 0x00000004;
    const TUint32 KGlxDownloadLink          = 0x00000005;
    const TUint32 KGlxDownloadLinkIcon      = 0x00000006;
    const TUint32 KGlxDownloadLinkText      = 0x00000007; 
	}

// CLASS DECLARATION

/**
 * GlxSettingsModel class 
 * 
 */
NONSHARABLE_CLASS( CGlxSettingsModel ) : public CBase
    {
    public:
        /**
        * Possible values for the "supported orientations" CR key.
        */
        enum TSupportedOrientations
            {
            ELandscapeAndPortrait = 1,
            ELandscape = 2
            };

    public:
        /**
        * Get singleton instance of the settings model.
        */ 
        IMPORT_C static CGlxSettingsModel* InstanceL();
        /**
        * Release instance of the settings model.
        */ 
        IMPORT_C void Close();
        /**
        * Destructor.
        */ 
        ~CGlxSettingsModel();

        /**
        * Get the supported screen orientations for the application.
        */
        IMPORT_C TSupportedOrientations SupportedOrientations();
        /**
        * Does screen orientation change to portrait for text entry?
        */
        IMPORT_C TBool IsTextEntryPortrait();
        /**
        * Is "view in portrait/landscape" available in the toolbar?
        */
        IMPORT_C TBool ShowRotateInToolbar();
        /**
        * Is slideshow available in the toolbar?
        */
        IMPORT_C TBool ShowSlideshowInToolbar();

        /**
         * DEPRECATED - Introduce a new function instead of using this for more things
         * Retrieves the settings value for the attribute identifier
         * @param aSettingsId the identity of the setting
         * @return The setting value
         */
        template <typename T>
        /* Deprecated */ static inline T ValueL(const TUint32& aSettingsId);
        
    
        /**
         * DEPRECATED - Introduce a new function instead of using this for more things
         * Persist the value of a gallery setting. 
         * @param aSettingsId The settings identifier for which the value is 
         *          persisted
         * @param aValue The value that is written
         */
        template<typename T> 
        /* Deprecated */ static inline void SetValueL(const TUint32& aSettingsId, T aValue);

    private:
        /**
        * Two phase constructor.
        */ 
        static CGlxSettingsModel* NewL();
        /**
        * Default constructor.
        */ 
        CGlxSettingsModel();
        /**
        * 2nd phase constructor.
        */ 
        void ConstructL();

    private:
        /** Whether feature manager was successfully initialised */
        TBool iFeatureManagerInitialised;
        /** Central repository for reading settings */
        CRepository* iRepository;
    };

#include "glxsettingsmodel.inl"    

#endif // _GLXSETTINGSMDL_H_
