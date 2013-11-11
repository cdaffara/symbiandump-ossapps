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
* Description:  This class provide API to read resource from resource file
*
*/


#ifndef CMPXRESOURCE_H
#define CMPXRESOURCE_H

//INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <barsc.h>

// CLASS DECLARATION

/**
*  Resource class
*  Singleton for one thread
*/
class CMPXResource : public CBase
    {
    public: // Factory and Release

        /**
        * Factory function, create the object.
        * Note that calls to this must be matched with a call to Release().
        *
        * @param aResourceFile the resource file
        *        In order to support installation of individual plugin.
        *        aResourceFile must be a final name, including full file path.
        *        All parse should be done in the plugin side.
        *
        * @return the resource object pointer
        */
        IMPORT_C static CMPXResource* NewL(const TDesC& aResourceFile);

        /**
        * Factory function, create the object.
        * Note that calls to this must be matched with a call to Release().
        * @param aResourceFile the resource file
        *        In order to support installation of individual plugin.
        *        aResourceFile must be a final name, including full file path.
        *        All parse should be done in the plugin side.
        *
        * @return the resource object pointer
        */
        IMPORT_C static CMPXResource* NewLC(const TDesC& aResourceFile);

        /**
        * Decrements the reference count, and delete the object if it is 0
        */
        IMPORT_C void Release();

    public: // New functions

        /**
        * Read array of descriptors
        * @param aResourceId resource id
        * @return array of descriptors. Ownership is abandoned.
        */
        IMPORT_C CDesCArrayFlat* ReadDesCArrayL(TInt aResourceId);

        /**
        * Read array of descriptors
        * @param aResourceId resource id
        * @return array of descriptors. Ownership is abandoned.
        */
        IMPORT_C CDesCArrayFlat* ReadDesCArrayLC(TInt aResourceId);

        /**
        * Get a heap descriptor from the resource file
        * @param aResourceId resource id
        * @return pointer to the heap descriptor.  Ownership is abandoned.
        */
        IMPORT_C HBufC* ReadHBufCL(TInt aResourceId);

        /**
        * Read array of menu items
        * @param aResourceId resource id
        * @param aIdArray array of Id within the specified resource
        * @return array of descriptors
        */
        IMPORT_C CDesCArrayFlat* ReadMenuArrayL(TInt aResourceId, RArray<TInt>& aIdArray);

        /**
        * Read array of menu items
        * @param aResourceId resource id
        * @param aIdArray array of Id within the specified resource
        * @return array of descriptors
        */
        IMPORT_C CDesCArrayFlat* ReadMenuArrayLC(TInt aResourceId, RArray<TInt>& aIdArray);


    private:

        /**
        * C++ constructor.
        */
        CMPXResource();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC& aResourceFile);

        /**
        * Don't allow users to delete this object
        * Release has to be called
        */
        virtual ~CMPXResource();


    private: // Data
        // Member variables
        RFs               iFs;
        RResourceFile     iResourceFile;
    };

#endif // CMPXRESOURCE_H

// End of File
