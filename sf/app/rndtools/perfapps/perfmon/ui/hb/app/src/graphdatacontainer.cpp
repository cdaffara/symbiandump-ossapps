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

#include "graphdatacontainer.h"
#include "enginewrapper.h"

#include <QPainter>
#include <QFontMetrics>

const int amountOfMicroSecondsFitsScreen = 20 * 1000000;
const int microToSecondMultiplier = 1000000;

GraphDataContainer::GraphDataContainer(const EngineWrapper& engine, QGraphicsItem *parent) :
    DataContainer(engine, parent)
{
    HbFontSpec spec(HbFontSpec::Secondary);
    mFont = spec.font();
    mFont.setPixelSize(12);
}

void GraphDataContainer::paint (QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // set proper font and prepare font metrics for text width / height calculation
    painter->setFont(mFont);
    QFontMetricsF metrics(mFont);

    // fill background with black color
    painter->fillRect(rect(), Qt::black);

    // calculate time factor
    qreal scaleFactor = rect().width() / (qreal) amountOfMicroSecondsFitsScreen;

    // calculate area height which is used to draw the grpahs
    qreal drawAreaHeight = rect().height() - 2 * metrics.height();


    const QList<SampleEntry> sampleEntries = engine().sampleEntries();

    // check if sample array has been constructed
    if (sampleEntries.length())
    {
        // draw vertical time lines first
        int verticalBarPeriodInSecs = engine().settings().graphVerticalBarPeriod();

        if (verticalBarPeriodInSecs >= 1 && sampleEntries.first().sampleCount() > 0)
        {
            // get time from the first sample
            const SampleData& firstSample = sampleEntries.first().sample(0);
            qint64 currentMicroSeconds = firstSample.mTimeFromStart;

            // calculate amount of microseconds exceeding value by using the modulo operator
            int remainderInMicroSeconds = currentMicroSeconds % (verticalBarPeriodInSecs * 1000000);

            // calculate first x pos
            qreal vbarXpos = rect().width() - (remainderInMicroSeconds * scaleFactor);

            // calculate the amount in seconds
            int barSeconds = (currentMicroSeconds - remainderInMicroSeconds) / microToSecondMultiplier;

            // continue drawing periodically the vertical lines
            while (vbarXpos >= 0 && barSeconds >= 0)
            {
                // draw vertical line
                painter->setPen(Qt::darkRed);
                painter->drawLine(QPointF(vbarXpos, metrics.height() + 1),
                                  QPointF(vbarXpos, rect().height() - metrics.height()));

                // draw seconds value
                painter->setPen(Qt::darkGray);
                QString secsText = QString("%1s").arg(barSeconds);
                QPointF secsPos(vbarXpos - metrics.width(secsText) / 2,
                                rect().height());
                painter->drawText(secsPos, secsText);

                // calculate new position
                vbarXpos -= verticalBarPeriodInSecs * 1000000 * scaleFactor;
                barSeconds -= verticalBarPeriodInSecs;
            }
        }

        // draw the basic grid
        painter->setPen(Qt::darkGray);

        qreal axisY = metrics.height();
        painter->drawLine(QPointF(0, axisY), QPointF(rect().width(), axisY)); // upper line
        painter->drawText(QPointF(0, axisY), tr("100%"));

        axisY = rect().height() / 2;
        painter->drawLine(QPointF(0, axisY), QPointF(rect().width(), axisY)); // mid line
        painter->drawText(QPointF(0, axisY), tr("50%"));

        axisY = rect().height() - metrics.height();
        painter->drawLine(QPointF(0, axisY), QPointF(rect().width(), axisY)); // bottom line
        painter->drawText(QPointF(0, axisY), tr("0%"));

        int c = 0;
        // draw graphs for each sampled type
        for (int i=0; i<sampleEntries.length(); i++)
        {
            // check if this setting has been enabled and it has some data
            if (engine().settings().graphSources().isEnabled(engine().SampleEntryPosToSettingPos(i)) &&
                sampleEntries.at(i).sampleCount() > 0)
            {
                // set pen color for the graph
                painter->setPen(sampleEntries.at(i).graphColor());

                // remember the position where drawing started
                qreal currentXPos(rect().width()); // start drawing from right
                qreal currentYPos(0.);

                // draw samples
                for (int j=0; j<sampleEntries.at(i).sampleCount()-1; j++)
                {
                    const SampleData& currentSample = sampleEntries.at(i).sample(j);
                    const SampleData& previousSample = sampleEntries.at(i).sample(j+1);

                    // calculate X position for previous (j+1)
                    qreal previousXPos = currentXPos -
                        qAbs(previousSample.mTimeFromStart - currentSample.mTimeFromStart) * scaleFactor;


                    // calculate initial Y position
                    if (j==0)
                    {
                        currentYPos = currentSample.mSize > 0 ?
                            drawAreaHeight * currentSample.mFree / currentSample.mSize + metrics.height() :
                            rect().height() - metrics.height();
                    }

                    // calculate Y position for previous (j+1)
                    qreal previousYPos = previousSample.mSize > 0 ?
                        drawAreaHeight * previousSample.mFree / previousSample.mSize + metrics.height() :
                        rect().height() - metrics.height();


                    // draw a line between the previous and current
                    painter->drawLine(QPointF(previousXPos, previousYPos),
                                      QPointF(currentXPos, currentYPos));

                    // draw current value in %
                    if (j==0) // draw the value of first sample
                    {
                        qreal perc = currentSample.mSize > 0 ?
                                     100. - 100. * currentSample.mFree / currentSample.mSize : 0;
                        QString percText = QString ("%1 %2%").
                                           arg(sampleEntries.at(i).description()).
                                           arg(perc, 0, 'f', 0);

                        painter->drawText(QPointF(0, metrics.height() * (c + 2)),
                                          percText);
                        c++;
                    }


                    // stop drawing if we have run out of space
                    if (previousXPos < 0)
                        break;

                    // remeber previous values
                    currentXPos = previousXPos;
                    currentYPos = previousYPos;
                }


            }
        }

    }
}
