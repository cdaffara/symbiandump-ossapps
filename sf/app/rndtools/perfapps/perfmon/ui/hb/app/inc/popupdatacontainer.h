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

#ifndef POPUPDATACONTAINER_H
#define POPUPDATACONTAINER_H

#include "datacontainer.h"

#include <QFont>

class PopupDataContainer : public DataContainer
{
    Q_OBJECT
public:
    PopupDataContainer(const EngineWrapper& engine, QGraphicsItem *parent = 0);

public slots:
    virtual void samplesUpdated();

protected:
    void paint (QPainter *painter,
                const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0);

private:
    QFont mFont;
};

#endif // POPUPDATACONTAINER_H
