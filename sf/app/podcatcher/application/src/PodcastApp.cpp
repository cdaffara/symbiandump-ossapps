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

#include    "PodcastApp.h"
#include    "PodcastDocument.h"

TUid CPodcastApp::AppDllUid() const
    {
    return KUidPodcast;
    }

CApaDocument* CPodcastApp::CreateDocumentL()
    {
    return CPodcastDocument::NewL( *this );
    }

