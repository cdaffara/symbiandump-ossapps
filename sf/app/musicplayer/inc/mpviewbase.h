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
* Description: Base class for all Music Player views.
*
*/

#ifndef MPVIEWBASE_H
#define MPVIEWBASE_H

#include <QString>
#include <hbview.h>
#include "mpcommondefs.h"

class MpViewBase : public HbView
{
    Q_OBJECT

public:

    void setViewMode(MpCommon::MpViewMode viewMode) { mViewMode = viewMode; }
    MpCommon::MpViewMode viewMode() { return mViewMode; }

signals:

    void songSelected( QString songPath );

protected:

    MpViewBase() : mViewMode(MpCommon::DefaultView) {}

protected:

    MpCommon::MpViewMode    mViewMode;

};

#endif  // MPVIEWBASE_H

