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
*
*/

#ifndef NMSETTINGSPLUGIN_H
#define NMSETTINGSPLUGIN_H

#include <cpplugininterface.h>
#include <qobject.h>

class CpSettingFormItemData;
class CpItemDataHelper;
class QTranslator;

class NmSettingsPlugin : public QObject, public CpPluginInterface
    {
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)

public:

    NmSettingsPlugin();

    virtual ~NmSettingsPlugin();

    virtual QList<CpSettingFormItemData*> createSettingFormItemData(
        CpItemDataHelper &itemDataHelper) const;

private:

    Q_DISABLE_COPY(NmSettingsPlugin)

private: // data

    // Owned.
    QTranslator *mTranslator;

    };

#endif // NMSETTINGSPLUGIN_H
