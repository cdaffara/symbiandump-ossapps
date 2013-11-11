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

// Version : %version:  2 %


#ifndef CVIDEOCONTAINER_H
#define CVIDEOCONTAINER_H

#include <coecntrl.h>


class CVideoContainer : public CCoeControl
{

    public:
        CVideoContainer();
        virtual ~CVideoContainer();

        void ConstructL();
};

#endif  // CVIDEOCONTAINER_H

// End of file