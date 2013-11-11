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



#ifndef PROFILER_GUI_DOCUMENT_H
#define PROFILER_GUI_DOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class CEikAppUi;
class CProfilerGuiModel;


// CLASS DECLARATION

/**
*  CProfilerGuiDocument application class.
*/
class CProfilerGuiDocument : public CAknDocument
    {
    public: // Constructors and destructor
        static CProfilerGuiDocument* NewL(CEikApplication& aApp);
        virtual ~CProfilerGuiDocument();

    public: // New functions

    public:	// from CEikDocument

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CProfilerGuiDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CProfilerGuiAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

    public:
        inline CProfilerGuiModel* Model() { return iModel; }

    private:
        CProfilerGuiModel* iModel;

    };

#endif

// End of File

