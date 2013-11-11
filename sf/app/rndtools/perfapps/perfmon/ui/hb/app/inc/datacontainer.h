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

#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <hbwidget.h>
#include <enginewrapper.h>

class DataContainer : public HbWidget
{
    Q_OBJECT

public:
    DataContainer(const EngineWrapper& engine, QGraphicsItem *parent = 0) :
            HbWidget(parent),
            mEngine(engine)
    {
    }

    void hideContainer()
        {
        disconnect(&mEngine, SIGNAL(samplesUpdated()), this, SLOT(samplesUpdated()));
        hide();
        }
    
    void showContainer()
        {
        connect(&mEngine, SIGNAL(samplesUpdated()),this, SLOT(samplesUpdated()));
        show();
        }
    
    inline const EngineWrapper& engine() const { return mEngine; }

public slots:
    virtual void samplesUpdated()
    {
    update();
    }

private:
    const EngineWrapper& mEngine;
};

#endif // DATACONTAINER_H
