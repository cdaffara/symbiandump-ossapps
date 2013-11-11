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

#include <QtDebug>
#include <QIcon>
#include <QVariant>
#include <QList>
#include <HbMainWindow>

#include "perfmondatapopupdialog_p.h"
#include "perfmondatapopuppluginerrors_p.h"
#include "perfmondatapopupwidget_p.h"

// Constructor
PerfMonDataPopupDialog::PerfMonDataPopupDialog(const QVariantMap &parameters) :
        mLastError(NoError),
        mShowEventReceived(false),
        mLocation(LocationUndefined),
        mWidget(new PerfMonDataPopupWidget(this))
{
    setTimeout(0);
    setModal(false);
    setDismissPolicy(HbPopup::NoDismiss);
    setBackgroundItem(0);
    setContentWidget(mWidget);
    setBackgroundFaded(false);

    setDeviceDialogParameters(parameters);
}

PerfMonDataPopupDialog::~PerfMonDataPopupDialog()
{
}

// Set parameters
bool PerfMonDataPopupDialog::setDeviceDialogParameters(const QVariantMap &parameters)
{
    if (parameters.contains("lines"))
    {
        QVariant lines = parameters.value("lines");
        if (!lines.canConvert(QVariant::StringList)) {
            mLastError = ParameterError;
            return false;
        }

        setLines(lines.toStringList());
    }

    if (parameters.contains("location"))
    {
        QVariant location = parameters.value("location");
        if (!location.canConvert<int>()) {
            mLastError = ParameterError;
            return false;
        }

        if (location.toInt() != LocationTopRight &&
            location.toInt() != LocationBottomMiddle)
        {
            mLastError = ParameterError;
            return false;
        }

        setLocation(static_cast<Location>(location.toInt()));
    }
    update();
    return true;
}

// Get error
int PerfMonDataPopupDialog::deviceDialogError() const
{
    return mLastError;
}

// Close device dialog
void PerfMonDataPopupDialog::closeDeviceDialog(bool byClient)
{
    Q_UNUSED(byClient);
    close();
    // If show event has been received, close is signalled from hide event. If not,
    // hide event does not come and close is signalled from here.
    if (!mShowEventReceived) {
        emit deviceDialogClosed();
    }
}

// Return display widget
HbPopup *PerfMonDataPopupDialog::deviceDialogWidget() const
{
    return const_cast<PerfMonDataPopupDialog*>(this);
}

// Widget is about to hide. Closing effect has ended.
void PerfMonDataPopupDialog::hideEvent(QHideEvent *event)
{
    HbPopup::hideEvent(event);
    emit deviceDialogClosed();
}

// Widget is about to show
void PerfMonDataPopupDialog::showEvent(QShowEvent *event)
{
    HbPopup::showEvent(event);
    mShowEventReceived = true;
}


void PerfMonDataPopupDialog::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HbDialog::mousePressEvent(event);
    
    QVariantMap data;
    data["mouseEvent"] = "press";
    emit deviceDialogData(data);
}


PerfMonDataPopupDialog::Location PerfMonDataPopupDialog::location() const
{
    return mLocation;
}

void PerfMonDataPopupDialog::setLocation(Location location)
{
    if (location != mLocation) {
        mLocation = location;
        QSize screenSize = HbDeviceProfile::profile(mainWindow()).logicalSize();
        if(mLocation == LocationTopRight) {
            setPreferredPos(QPointF(screenSize.width(), 0), HbPopup::TopRightCorner);
        }
        else{
            setPreferredPos(QPointF(screenSize.width() / 2, screenSize.height()), HbPopup::BottomEdgeCenter);
        }
    }
}

QStringList PerfMonDataPopupDialog::lines() const
{
    return mWidget->lines();
}

void PerfMonDataPopupDialog::setLines(const QStringList &lines)
{
    mWidget->setLines(lines);
}
