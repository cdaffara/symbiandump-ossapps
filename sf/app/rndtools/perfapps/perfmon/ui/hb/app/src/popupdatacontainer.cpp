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

#include "popupdatacontainer.h"
#include "enginewrapper.h"

#include <QGraphicsLinearLayout>
#include <HbLabel>
#include <QPainter>

const int leftMargin = 2;

PopupDataContainer::PopupDataContainer(const EngineWrapper& engine, QGraphicsItem *parent) :
    DataContainer(engine, parent)
{
    connect(&engine, SIGNAL(samplesUpdated()), this, SLOT(samplesUpdated()));

    HbFontSpec spec(HbFontSpec::Secondary);
    mFont = spec.font();
    mFont.setPixelSize(12);
    
}

void PopupDataContainer::samplesUpdated()
{
    update();
}

void PopupDataContainer::paint (QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // set proper font and prepare font metrics for text height calculation
    painter->setFont(mFont);
    QFontMetricsF metrics(mFont);

    QList<SampleEntry> entries = engine().sampleEntries();
    qreal verticalPos = metrics.height();

    for (int i=0; i<entries.length(); i++)
    {
        if (engine().settings().dataPopupSources().isEnabled(i) &&
            entries.at(i).sampleCount() > 0)
        {
            const SampleEntry &entry = entries.at(i);
            const SampleData &sample = entry.sample(0);

            if (i == ESourceCPU)
            {
                // CPU
                double perc = sample.mSize > 0 ?
                              100. - 100. * sample.mFree / sample.mSize : 0;


                QString text = tr("%1 %2%").arg(entry.description()).
                               arg(perc, 0, 'f', 2);
                painter->drawText(QPointF(leftMargin, verticalPos), text);
            }
            else if (i == ESourcePwr)
            {
                // Power
                QString text = tr("%1 %L2%3").arg(entry.description()).
                               arg(sample.mSize - sample.mFree).arg(entry.unitShort());
                painter->drawText(QPointF(leftMargin, verticalPos), text);
            }
            else
            {
                // RAM and Drives
                QString text = tr("%1 free %L2%3").arg(entry.description()).
                               arg(sample.mFree).arg(entry.unitShort());
                painter->drawText(QPointF(leftMargin, verticalPos), text);
            }

            verticalPos += metrics.height();
        }
    }
}
