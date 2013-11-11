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


#include "ctemplatedata.h"

//  CONSTRUCTION
CTemplateData* CTemplateData::NewL()
    {    
    CTemplateData* self = CTemplateData::NewLC();
    CleanupStack::Pop();    // self
    return self;
    }

CTemplateData* CTemplateData::NewLC()
    {
    CTemplateData* self = new ( ELeave ) CTemplateData;
    CleanupStack::PushL( self );
    return self;
    }

// Default constructor
CTemplateData::CTemplateData()
    {
    }

// Destructor
CTemplateData::~CTemplateData()
    {
    }

// Clones single template data instance
CTemplateData* CTemplateData::CloneL()
    {
    CTemplateData* self = CTemplateData::NewL();
    self->iUid = iUid;
    return self;
    }

//  End of File
