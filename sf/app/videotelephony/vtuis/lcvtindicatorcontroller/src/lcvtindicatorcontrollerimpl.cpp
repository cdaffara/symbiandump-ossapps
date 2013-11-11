/*!
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
* Description:  Handles phone indicators.
*
*/

#include <QList>
#include <QPair>
#include <qglobal.h>
#include <hbindicator.h>
#include <hbindicatorinterface.h>
#ifdef Q_OS_SYMBIAN
#include <hbicon.h>
#endif
#include "lcvtindicatorcontrollerimpl.h"
#include <QDebug>

const static QString lcVtIndicator = "com.nokia.hb.indicator.lcvtindicatorplugin/1.0";


LcVtIndicatorControllerImpl::LcVtIndicatorControllerImpl()
{
    qDebug() << "LcVtIndicatorControllerImpl::LcVtIndicatorControllerImpl()";
    mIndicator = new HbIndicator();
}

LcVtIndicatorControllerImpl::~LcVtIndicatorControllerImpl()
{   
    qDebug() << "LcVtIndicatorControllerImpl::~LcVtIndicatorControllerImpl()";
    delete mIndicator;
}

void LcVtIndicatorControllerImpl::enableActiveCallIndicator()
{
    qDebug() << "LcVtIndicatorControllerImpl::enableActiveCallIndicator()";
    QString indicatorType(lcVtIndicator);
    QVariantMap parameters;
    
    //indicator text
    parameters.insert(
            QVariant(HbIndicatorInterface::PrimaryTextRole).toString(),
            hbTrId("txt_vt_dblist_ongoing_video_call"));
    
    //status bar icon
    parameters.insert(
            QVariant(HbIndicatorInterface::MonoDecorationNameRole).toString(),
            QString("qtg_mono_call"));
    
    //indicator icon
    parameters.insert(QVariant(HbIndicatorInterface::DecorationNameRole).toString(),
            QString("qtg_mono_call"));
    
    mIndicator->activate(indicatorType, parameters);
    qDebug() << "LcVtIndicatorControllerImpl::enableActiveCallIndicator(), exit";
}

void LcVtIndicatorControllerImpl::disableActiveCallIndicator()
{
    qDebug() << "LcVtIndicatorControllerImpl::disableActiveCallIndicator()";
    QString indicatorType(lcVtIndicator);
    mIndicator->deactivate(indicatorType);
}

// End of file

