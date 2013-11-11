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

#ifndef MEMSPYDOCUMENT_H
#define MEMSPYDOCUMENT_H

// System includes
#include <AknDoc.h>
   
// Classes referenced
class CMemSpyEngine;
class CEikAppUi;
class CMemSpySettings;
//cigasto
class RMemSpySession;


class CMemSpyDocument : public CAknDocument
    {
public: // Constructors and destructor
    static CMemSpyDocument* NewL(CEikApplication& aApp);
    ~CMemSpyDocument();

private:
    CMemSpyDocument(CEikApplication& aApp);
    void ConstructL();

public:
//    CMemSpyEngine& Engine();
//    const CMemSpyEngine& Engine() const;
    CMemSpySettings& Settings();
    const CMemSpySettings& Settings() const;

private: // Framework
    CEikAppUi* CreateAppUiL();

private: // Data members
//    CMemSpyEngine* iEngine;
    CMemSpySettings* iSettings;
    //cigasto
    RMemSpySession* iMemSpySession;
    };


#endif
