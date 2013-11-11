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

#ifndef DATAPOPUP_H
#define DATAPOPUP_H

#include <HbDeviceDialog>
#include <QFont>

class EngineWrapper;
class HbMainWindow;

class DataPopup : public HbDeviceDialog
{
    Q_OBJECT

public:
    DataPopup(EngineWrapper &engine, HbMainWindow &mainWindow);

public slots:
    void show();
    void hide();
    void updateSamples();
    void updateSettings();
    void updateVisibility(bool foreground);


private slots:
    void triggerAction(QVariantMap data);
    void orientationChanged(Qt::Orientation newOrientation);
    void inForeground();
    void inBackground();

private:
    QVariantMap collectParams() const;
    void updateData();

private:
    EngineWrapper& mEngine;
    HbMainWindow& mMainWindow;
    bool mPopupCreated;

    bool mPopupVisible;
};

#endif // DATAPOPUP_H
