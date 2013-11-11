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

#ifndef DEVICEMANAGEMENTNOTIFIERWIDGET_P_H
#define DEVICEMANAGEMENTNOTIFIERWIDGET_P_H

#include <QObject>
#include <QVariantMap>

#include <hbdevicedialoginterface.h>
#include <hbdevicedialog.h>
#include "devicemanagementnotifierutils.h"

class HbDialog;

class devicemanagementnotifierwidget :
    public devicemanagementnotifierutils, public HbDeviceDialogInterface
{
	Q_OBJECT

public:
enum ENotfierType
    {
	    EServerAlertNote,
	    EConnectingNote,
	    EInformativeNote,
	    EConfirmativeNote
    };
public:
	devicemanagementnotifierwidget(const QVariantMap &parameters);
	virtual ~devicemanagementnotifierwidget();

public: //derived from HbDeviceDialogInterface
	bool setDeviceDialogParameters(const QVariantMap &parameters);
    int deviceDialogError() const;
    void closeDeviceDialog(bool byClient);
    HbDialog *deviceDialogWidget() const;
    void dmDevdialogDismissed(ENotfierType notifiertype, int response);

signals:
        void deviceDialogClosed();
        void deviceDialogData(QVariantMap data);
 };

#endif // devicemanagementnotifierwidget_P_H
