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
* Description:  Video Source subsystem.
*
*/


#ifndef CVSCONTROLLERIMP_H
#define CVSCONTROLLERIMP_H

// INCLUDE FILES
#include "CApiVideoSource.h"

// CLASS DECLARATIONS

/**
*  Internal implementation of controller. This class is inherited from
*  public controller interface.
*
*  @lib videosource.lib
*/
class CVSControllerImp : public CVSController
    {
    public: // Constructors and destructor

        /**
        * Static factory function to create controller instance.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new controller instance.
        */
        static CVSControllerImp* NewL();

        /**
        * Destructor.
        */
        ~CVSControllerImp();

    public: // New functions

    public: // Functions from base classes

        /**
        * Create data source instance.
        * @param "aObserver" Pointer to data source observer.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new data source instance.
        */
        virtual CVSDataSource* CreateDataSourceL( 
            MVSDataSourceObserver* aObserver );

        /**
        * Return provider count and provider index.
        * @return Number of providers available. Count also acts as zero 
        * based provider index.
        */
        virtual TInt ProvidersAvailable();

        /**
        * Return provider information.
        * @param "aProviderIndex" Index of the provider to return information 
        * for.
        * @param "aInfo" Reference to the variable where the provider 
        * information is copied.
        * @exception Can leave with one of the system wide error codes.
        */
        virtual void ProviderInfoL( 
            TInt aProviderIndex, 
            TVSDataProviderInfo& aInfo );

        /**
        * Create provider instance.
        * @param "aProviderIndex" Index of the provider to be created.
        * @param "aObserver" Pointer to provider observer.
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new provider instance.
        */
        virtual CVSDataProvider* CreateDataProviderL( 
            TInt aProviderIndex, 
            MVSDataProviderObserver* aObserver, 
            MVSBufferPool* aPool );

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // Constructors and destructor

        /**
        * 2nd phase constructor.
        * @param "aObserver" Pointer to controller observer
        * @exception Can leave with one of the system wide error codes.
        */
        void ConstructL( MVSControllerObserver* aObserver );

    private:    // New functions

    private:    // Functions from base classes

    public: // Data

    protected:  // Data

    private:    // Data

    };

#endif      // CVSCONTROLLERIMP_H  
            
// End of File
