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

#ifndef PODCASTAPP_H
#define PODCASTAPP_H

#include <aknapp.h>

const TUid KUidPodcast = { 0xA0009D00 };

class CPodcastApp : public CAknApplication
    {
    private:

        CApaDocument* CreateDocumentL();
        TUid AppDllUid() const;
    };

#endif
