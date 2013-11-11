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

#ifndef CNTUISOCIALEXTENSION_H
#define CNTUISOCIALEXTENSION_H

#include <QObject>
#include <cntviewparams.h>

/**
 * UI extension class that supplies social extension action handling
 */
class CntUiSocialExtension
{
 public:
    /**
     * Handle view switching logic for toolbar actions
     *
     * @param CntViewParameters&, callback for view switching
     */
    virtual void handleToolbarAction(CntViewParameters& callBack) = 0;

protected:
    // prevent deleting by client
    virtual ~CntUiSocialExtension() {}
};

#endif //CNTUISOCIALEXTENSION_H

