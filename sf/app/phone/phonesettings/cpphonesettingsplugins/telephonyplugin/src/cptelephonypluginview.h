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

#ifndef CPTELEPHONYPLUGINVIEW_H_
#define CPTELEPHONYPLUGINVIEW_H_

#include <QObject>
#include <QVariantList>
#include <hbdataform.h>
#include <cpbasesettingview.h>

class HbDataFormModelItem;
class CpSettingFormItemData;
class CpPluginInterface;

// Class declaration 
class CpTelephonyPluginView : public CpBaseSettingView
{
    Q_OBJECT

public:
    CpTelephonyPluginView();
    CpTelephonyPluginView(const QVariantList &params);
    ~CpTelephonyPluginView();

private: 
    CpItemDataHelper* initializeItemDataHelper();
    QList<CpSettingFormItemData*> groupItemFromPlugin(
            const QString& plugin);
    
private: // Data
    CpItemDataHelper *m_helper;
    QList<CpPluginInterface*> m_plugins;
};

#endif /* CPTELEPHONYPLUGINVIEW_H_ */
