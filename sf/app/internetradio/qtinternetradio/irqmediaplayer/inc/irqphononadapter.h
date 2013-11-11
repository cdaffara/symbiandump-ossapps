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
#ifndef IRQPHONONADAPTER_H_
#define IRQPHONONADAPTER_H_

#include <QObject>
#include "irqplayeradapterinterface.h"

/**
 * This class wraps the PHONON interface
 */
class IRQPHONONAdapter : public IRQPlayerAdapterInterface
{
    Q_OBJECT

public:
    IRQPHONONAdapter();
    ~IRQPHONONAdapter();

    // Inherited from IRQPlayerAdapterInterface
    void  playStation(const QString &aUrl, int aApId);
    void  stop();
    void  setVolume(int aVolume);
    int   getVolume();
    void* getPlayerInstance();
    void  enableStereoEffect();
    void  disableStereoEffect();
};

#endif /* IRQPHONONADAPTER_H_ */
