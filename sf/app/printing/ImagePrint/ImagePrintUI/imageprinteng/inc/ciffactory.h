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


#ifndef CIFFACTORY_H
#define CIFFACTORY_H

#include <e32def.h>
#include <e32base.h>

class CImagePrintEngine;
class CImagePrint;
class MDiscovery;
class MPrintJob;
class MPrintPreview;
class MPrintSettings;
class MIdlePrintEventRegisterIF;
class RImagePrintClient;
class MPrintEventObserver;
class MPrintEventRegisterIF;

//  CLASS DEFINITION
/**
 *
 * Base class for the factories that create the concrete classes
 *
 */
class CIFFactory
    : public CBase
    {
    public:     // Constructors and destructors  
    
        /**
         *  Destructor
         */
        virtual ~CIFFactory();      

    protected:      // Constructors and destructors

        /**
         *  Constructor
         */
        CIFFactory( CImagePrintEngine* aEngine );
        
    public:         // New methods
                
        /**
         *  Returns discovery interface
         *
         *  @return Device discovery interface
         */
        virtual MDiscovery* DiscoveryIF() = 0;

        /**
         *  Returns settings interface
         *
         *  @return Settings interface
         */
        virtual MPrintSettings* SettingsIF() = 0;

        /**
         *  Returns print job interface
         *
         *  @return Print job interface
         */
        virtual MPrintJob* PrintJobIF() = 0;

        /**
         * Restarts Engine
         */
        virtual void RestartEngine() = 0;

        /**
         * Returns Engine
         */
        virtual CImagePrint* Engine() = 0;

        /**
         * Returns Browser preview interface
         */

         /**
         * Returns idle print event register interface
         */        
        virtual MIdlePrintEventRegisterIF* IdlePrintEventRegisterIF() = 0;
        
        /**
         * Returns Interface to RImagePrintClient
         * @return RImagePrintClient 
         */
        virtual RImagePrintClient ImagePrintClient() = 0;
        
        /**
         * Returns information about application status
         * @return TBool ETrue if application is already in use.
         */        
        virtual TBool IsApplicationAlreadyInUse() = 0;

        /**
         * Returns Interface to print event catcher
         * @return MPrintEventObserver 
         */
		virtual MPrintEventObserver* PrintEventObserverIF() = 0; 
		       
        /**
         * Returns Interface to print event register interface
         * @return MPrintEventRegisterIF 
         */
		virtual MPrintEventRegisterIF* PrintEventRegisterIF() = 0;
		
    protected:      // Data        

        CImagePrintEngine* iPrintEngine;
    };

#endif  //  CIFFACTORY_H

//  End of File
