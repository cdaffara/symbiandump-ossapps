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
* Description:  Monitor for call theme settings.
*
*/



#ifndef CPHONECALLTHEMEMONITOR_H
#define CPHONECALLTHEMEMONITOR_H

//  INCLUDES
#include    "mphonecenrepobserver.h" 

/**
*  Observer interface to call theme settings.
*
*/
class MPhoneCallThemeSettingsObserver
    {
    public:
        /**
        * Called when call theme settings have changed.
        */
        virtual void CallThemeSettingsChanged() = 0;
    };

// CLASS DECLARATION

/**
*  Monitor for call theme settings.
*
*/
class CPhoneCallThemeMonitor : public CBase, private MPhoneCenRepObserver
    {
    public:

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPhoneCallThemeMonitor* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPhoneCallThemeMonitor();
    
        /**
        * Fetches setting is call image theme .
        * @return value of settings
        */
        IMPORT_C TInt CallImageThemeSettings() const;
        
        /**
        * Fetches call image theme path.
        * @return Call image theme path.
        */
        IMPORT_C TPtrC CallImageThemeFilePath() const;

        /**
        * Set call theme settings observer.
        * @param aObserver Observer.
        */
        IMPORT_C void SetCallThemeSettingsObserver(
            MPhoneCallThemeSettingsObserver* aObserver );

    protected:
        
        /**
        * Handle the change of the setting from Central Repository
        * @param aUid identifing the central repository UID.
        * @param aId central repository ID.
        */
        virtual void HandleCenRepChangeL( 
            const TUid& aUid,
            const TUint aId );

 
    private:

        /**
        * C++ default constructor.
        */
        CPhoneCallThemeMonitor();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Reads all theme settings from CenRep.
        */
        void ReadAllL();
        
        
    private: // Data
        // Themes client id
        TInt iThemesClientID;
  
        // Current theme image setting
        TInt iThemeCallImageSettings;
        
        // Theme image path and name
        HBufC* iCallerImageThemePath;
        
        // Observer to be notified.
        MPhoneCallThemeSettingsObserver* iObserver;
     };

#endif      // CPHONECALLTHEMEMONITOR_H   
            
// End of File
