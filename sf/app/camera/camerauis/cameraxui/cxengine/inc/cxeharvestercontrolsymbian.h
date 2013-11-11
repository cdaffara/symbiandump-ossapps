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
#ifndef CXEHARVESTERCONTROLSYMBIAN_H_
#define CXEHARVESTERCONTROLSYMBIAN_H_

#include <QObject>
#include <QString>
#include <harvesterclient.h>
#include "cxeerror.h"


class CxeHarvesterControlSymbian : public QObject,
                                   private MHarvestObserver
{
    Q_OBJECT

public:

    CxeHarvesterControlSymbian();
    ~CxeHarvesterControlSymbian();

public slots:
    // slot that does the harvest file using the mds harvester
    CxeError::Id harvestFile(const QString& filename, bool addLocation, quint32 albumId);

signals:
    /**
    * Signal for file harvesting completed.
    * @param error Status code for the harvesting.
    * @param filename Path of the file that was harvested.
    */
    void fileHarvested(CxeError::Id error, const QString& filename);

private: // MHarvestObserver
    void HarvestingComplete(TDesC& aURI, TInt aError);


private: // helper methods
    void deRegisterHarverterClientEvents();
    void registerForHarvesterEvents();

private:
    RHarvesterClient mHarvesterClient;
    bool mHarvesterClientConnected;
    bool mRegisteredForHarvesterEvents;
};

#endif /* CXEHARVESTERCONTROLSYMBIAN_H_ */
