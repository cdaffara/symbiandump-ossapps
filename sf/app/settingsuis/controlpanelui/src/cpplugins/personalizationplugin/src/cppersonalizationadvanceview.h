/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef CPPERSONALIZATIONADVANCEVIEW_H
#define CPPERSONALIZATIONADVANCEVIEW_H

#include <cpbasesettingview.h>
class CpItemDataHelper;
class CpProfileSettingForm;

class CpPersonalizationAdvanceView: public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit CpPersonalizationAdvanceView(QGraphicsItem *parent = 0);
    ~CpPersonalizationAdvanceView();
    
private:
    void initMenu();
    
private:
    CpProfileSettingForm *mDataForm;
};
#endif
