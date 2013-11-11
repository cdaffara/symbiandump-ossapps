/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*    Spinner dialog shown when setting new background image
*
*/

#ifndef __SPINNERDIALOG_H
#define __SPINNERDIALOG_H

#include <HbDialog>
#include "hsutils_global.h"
#include "hstest_global.h"
class HbIconItem;
HOMESCREEN_TEST_CLASS(t_hsUtils)

class HSUTILS_EXPORT HsSpinnerDialog : public HbDialog
{
    Q_OBJECT
    
public: 
    HsSpinnerDialog(QGraphicsItem *parent=0);
    ~HsSpinnerDialog();
    
public slots:
    void start();
    void stop();
    
private:    
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // __SPINNERDIALOG_H
