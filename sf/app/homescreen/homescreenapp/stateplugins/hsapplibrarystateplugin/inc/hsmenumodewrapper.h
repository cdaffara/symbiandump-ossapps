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
 * Description: Menu View Builder
 *
 */

#ifndef HSMENUMODEWRAPPER_H
#define HSMENUMODEWRAPPER_H

#include <QVariant>
#include "hsapp_defs.h"

class HsMenuModeWrapper
{
public:
    HsMenuModeWrapper();
    
    Hs::HsMenuMode getHsMenuMode() const;
    QVariant getHsToken() const;

    void setHsMenuMode(Hs::HsMenuMode menuMode, QVariant token = NULL);

private:
    Hs::HsMenuMode mMode;
    QVariant mToken;
};

#endif
