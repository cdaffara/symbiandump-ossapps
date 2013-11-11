/*
* Copyright (c) 2007-2010 Sebastian Brannstrom, Lars Persson, EmbedDev AB
*
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* EmbedDev AB - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "PodcastDocument.h"
#include "PodcastAppui.h"
#include "PodcastModel.h"

CPodcastDocument::CPodcastDocument( CEikApplication& aApp )
    : CAknDocument( aApp )    
    {
    }

CPodcastDocument::~CPodcastDocument()
    {
    delete iPodcastModel;
    }

void CPodcastDocument::ConstructL()
    {
    DP("CPodcastDocument::ConstructL BEGIN");
    iPodcastModel = CPodcastModel::NewL();
    DP("CPodcastDocument::ConstructL END");
    }

CPodcastDocument* CPodcastDocument::NewL(CEikApplication& aApp )
    {
    CPodcastDocument* self = new (ELeave) CPodcastDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CEikAppUi* CPodcastDocument::CreateAppUiL()
    {
    return new (ELeave) CPodcastAppUi(iPodcastModel);
    }
