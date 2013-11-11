/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef PERFMON_DOCUMENT_H
#define PERFMON_DOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class CEikAppUi;
class CPerfMonModel;


// CLASS DECLARATION

/**
*  CPerfMonDocument application class.
*/
class CPerfMonDocument : public CAknDocument
    {
    public: // Constructors and destructor
        static CPerfMonDocument* NewL(CEikApplication& aApp);
        virtual ~CPerfMonDocument();

    public: // New functions

    public:	// from CEikDocument
	    CFileStore* OpenFileL(TBool aDoOpen,const TDesC& aFilename,RFs& aFs);
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CPerfMonDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CPerfMonAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

    public:
        inline CPerfMonModel* Model() { return iModel; }

    private:
        CPerfMonModel* iModel;

    };

#endif

// End of File

