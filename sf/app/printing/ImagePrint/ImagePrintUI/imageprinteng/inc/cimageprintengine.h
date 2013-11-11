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


#ifndef CIMAGEPRINTENGINE_H
#define CIMAGEPRINTENGINE_H

#include <e32def.h>
#include <badesca.h>

class CIFFactory;
class CRepository;

/**
 *
 * The main engine class, creates the factory that initializes the interface
 * implementations and other instances
 *
 */
class CImagePrintEngine
    : public CBase
    {
    public:     // Constructors and destructors

        /**
         *  Two-phase constructor
         *
         *  @param aImageFiles  The printed image files, ownership of the
         *                      array is transferred. Can be also NULL at this
         *                      stage
         *  @param aIdleObserver Observer to notify UI about idle events
         *
         *  @return Initialized instance of CImagePrintEngine
         */
        IMPORT_C static CImagePrintEngine* NewL(
                        CDesCArrayFlat* aImageFiles );
        IMPORT_C static CImagePrintEngine* NewLC(
                        CDesCArrayFlat* aImageFiles );

        /**
         *  Destructor
         */
        virtual ~CImagePrintEngine();

    protected:      // Constructors and destructors

        /**
         *  Constructor
         */
        CImagePrintEngine();

        /**
         *  Second phase constructor
         *
         *  @param aImageFiles  The printed image files, ownership of the
         *                      array is transferred
         *  @param aIdleObserver Observer to notify UI about idle events
         */
        void ConstructL( CDesCArrayFlat* aImageFiles );

    public:         // New methods

         /**
          *  Returns reference to the interface factory class that
          *  can be used to get the pointers to the concrete implementations
          *  of the mixin-interface classes
          *
          *  @return Reference to the initialized interface factory
          */
         IMPORT_C CIFFactory& InterfaceFactory();

         /**
          *  Sets the new image array. Ownership of the array is transferred
          *
          *  @param aArray New array
          */
         IMPORT_C void SetImageArrayL( CDesCArrayFlat* aArray );

         /**
          *  Returns the array of images
          *  @return array of images
          */
         IMPORT_C RPointerArray<TDesC>& FileArray();

         /**
          *  Restarts Image Print engine by deleting and reconstructing it.
          */
         IMPORT_C void RestartEngine();


    protected:      // New methods

        /**
         *  Creates new TFileName for file array
         *  @return pointer to new TFileName object
         */
        TFileName* CreateFileNameL();

    protected:      // Data

        // Printed image files
        CDesCArrayFlat* iFiles;

        // RArray variant of the files for server
        RPointerArray<TDesC> iFileArray;

        // Factory
        CIFFactory* iIFFactory;

		// A Central Repository session
        CRepository* iCRSession;
    };

#endif // CIMAGEPRINTENGINE_H

//  End of File
