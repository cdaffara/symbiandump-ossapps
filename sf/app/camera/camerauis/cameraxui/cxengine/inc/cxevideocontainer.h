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
#ifndef CXEVIDEOCONTAINER_H
#define CXEVIDEOCONTAINER_H

#include <coecntrl.h>


class CxeVideoContainer : public CCoeControl
{
public:
    CxeVideoContainer();
    virtual ~CxeVideoContainer();

    void ConstructL();

protected:
    void Draw(const TRect& aRect) const;
};




#endif  // CXEVIDEOCONTAINER_H

