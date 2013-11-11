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

#ifndef IRSETTINGSVIEW_H
#define IRSETTINGSVIEW_H

#include "irbaseview.h"

class HbDataForm;
class HbDataFormModel;

 

class IRSettingsView : public IRBaseView
{
    Q_OBJECT
public:
    ~IRSettingsView();
    
protected:
    IRSettingsView(IRApplication *aApplication, TIRViewId aViewId);
    
private slots:
    void toggleChange(QModelIndex aStartIn, QModelIndex aEndIn);
    
private:
    void initView();
    void setPreferredQuality();
    
private:
    HbDataForm *iForm;
    HbDataFormModel *iModel;
    friend class IRViewManager;
};

#endif
