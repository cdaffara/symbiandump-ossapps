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
* Description:  Videoteleng Camera Preferences extension 
*
*/


#ifndef CVTENGCAMERAPREFERENCES_H
#define CVTENGCAMERAPREFERENCES_H

//  INCLUDES
#include "VtEngPanic.h"
#include "CVtEngSettings.h"
#include "VtEngUtils.h"
#include "CVtEngOperation.h"

#include <cvtlogger.h>
#include <e32base.h>

#include <mvtengcamerapreferences.h>
#include "CVtEngExtensionBase.h"

// FORWARD DECLARATIONS
class CVSDataProvider;
class MVtEngCameraPreferencesObserver;

// CLASS DECLARATION

/**
*  Videoteleng extension base class.
*
*  @lib videoteleng.lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS ( CVtEngCameraPreferences ): public CVtEngExtensionBase,
											 public MVtEngCameraPreferences
    {
    public: //Constructor and destructor.
        
        /**
        * Symbian two-phase constructor.
        * @return engine extension instance
        */
        static CVtEngCameraPreferences* NewL();

        /**
        * Destructor.
        */
        ~CVtEngCameraPreferences();
        
    public: // from base class CVtEngExtensionBase
        
       	/**
        * @see CVtEngExtensionBase::GetInterface
        */
        virtual TAny* GetInterface();
        
       	/**
        * @see CVtEngExtensionBase::HandleL
        */
        virtual TBool HandleL( CVtEngOperation* aOp );
        
        /**
        * @see CVtEngExtensionBase::ValidateCommand
        */
        virtual TBool ValidateCommand( const TVtEngCommandId aCommandId );
        
    public: // from base class MVtEngCameraPreferences
        
        /**
        * @see MVtEngCameraPreferences::GetColorTone
        */
        virtual TInt GetColorTone( 
        	MVtEngCameraPreferences::TColorTone& aColortone ) const; 
        
        /**
        * @see MVtEngCameraPreferences::GetSupportedColorTones
        */
        virtual TInt GetSupportedColorTones( 
        	TUint32& aSupportedColorTones ) const;
        
        /**
        * @see MVtEngCameraPreferences::GetWhiteBalance
        */
        virtual TInt GetWhiteBalance( 
        	MVtEngCameraPreferences::TWhiteBalance& aWhiteBalance ) const;     
        
        /**
        * @see MVtEngCameraPreferences::GetSupportedWhiteBalanceModes
        */
        virtual TInt GetSupportedWhiteBalanceModes( 
        	TUint32& aWhiteBalance ) const; 
        
        /**
        * @see MVtEngCameraPreferences::GetBrightness
        */  
        virtual TInt GetBrightness( TInt& aBrightness) const; 
        
        /**
        * @see MVtEngCameraPreferences::GetContrast
        */  
        virtual TInt GetContrast( TInt& aContrast ) const;
    
    public:
           
        /**
        * Sets data provider to camera prefefrences
        * @param aProvider pointer to current active data provider
        * @param aObserver reference to observer
        */
        virtual void SetProvider( CVSDataProvider* aProvider,
            MVtEngCameraPreferencesObserver& aObserver );

        /**
        * Detaches observer from preference instance.
        * @param aObserver reference to observer
        */
        void Detach( MVtEngCameraPreferencesObserver& aObserver );

    private:
    
        /*
        * C++ default constructor.
        */        
        CVtEngCameraPreferences();
        
        /**
        * Symbian two-phase constructor.
        */
        void ConstructL();
    
    private:
        
        // Video source data provider pointer
        CVSDataProvider* iProvider;
            
        // Preference observer pointer
        MVtEngCameraPreferencesObserver* iObserver;

    };
        
#endif //CVTENGCAMERAPREFERENCES_H    