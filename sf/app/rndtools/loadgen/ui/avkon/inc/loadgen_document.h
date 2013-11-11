/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOADGEN_DOCUMENT_H
#define LOADGEN_DOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class CEikAppUi;
class CLoadGenModel;


// CLASS DECLARATION

/**
*  CLoadGenDocument application class.
*/
class CLoadGenDocument : public CAknDocument
    {
    public: // Constructors and destructor
        static CLoadGenDocument* NewL(CEikApplication& aApp);
        virtual ~CLoadGenDocument();

    public: // New functions

    public:// from CEikDocument
    CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CLoadGenDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CLoadGenAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

    public:
        inline CLoadGenModel* Model() { return iModel; }

    private:
        CLoadGenModel* iModel;

    };

#endif

// End of File

