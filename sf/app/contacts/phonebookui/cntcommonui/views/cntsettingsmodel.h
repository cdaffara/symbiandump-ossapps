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

#ifndef CNTSETTINGSMODEL_H
#define CNTSETTINGSMODEL_H

#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <xqsettingsmanager.h>

class XQSettingsKey;

/**
* Model class for populating the settings view with the value choices
*/
class CntSettingsModel : public HbDataFormModel
{
    Q_OBJECT
    
public:
    CntSettingsModel();
    ~CntSettingsModel();

private:
    void createNameOrderSettingsItem();
    void createNamesListRowSettingItem();

private slots:
    void handleDataChanged(QModelIndex topLeft, QModelIndex bottomRight);
    
private:
    XQSettingsManager mSettings;
    XQSettingsKey *mNameOrderkey;
    XQSettingsKey *mNameListRowSettingkey;
};

#endif
