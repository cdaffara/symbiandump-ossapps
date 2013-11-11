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
 * Description: DM indicator base class implementation
 *
 */
#include <QByteArray>
#include <QVariant>
#include <qtranslator.h>
#include <qcoreapplication.h>
#include <qlocale.h>
#include "dmindicator.h" 

// ----------------------------------------------------------------------------
// DMIndicator::DMIndicator
// @see DMIndicator.h
// ----------------------------------------------------------------------------
DMIndicator::DMIndicator(const QString &indicatorType,
        HbIndicatorInterface::Category category,
        HbIndicatorInterface::InteractionTypes interactionTypes) :
    HbIndicatorInterface(indicatorType, category, interactionTypes)
    {
    qDebug("DMIndicator::DMIndicator() start");
    mParameter = 0;

    // Do translation
    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_" + lang, path);
    if (fine)
        qApp->installTranslator(translator);
    qDebug("DMIndicator::DMIndicator() end");
    }

// ----------------------------------------------------------------------------
// DMIndicator::~DMIndicator
// @see DMIndicator.h
// ----------------------------------------------------------------------------
DMIndicator::~DMIndicator()
    {
    
    }

// ----------------------------------------------------------------------------
// DMIndicator::indicatorData
// @see DMIndicator.h
// ----------------------------------------------------------------------------
QVariant DMIndicator::indicatorData(int role) const
    {
    qDebug("DMIndicator::indicatorData() start");
    switch (role)
        {

        case MonoDecorationNameRole:
            {
            //QString iconName("z:/resource/devicemanagement/qtg_small_smiley_kissing.svg");
            //QString iconName("C:/resource/devicemanagement/qtg_small_smiley_kissing.svg");
            QString iconName("qtg_small_smiley_kissing");
            return iconName;
            }

        default:
            return QVariant();

        }
    }

// ----------------------------------------------------------------------------
// DMIndicator::handleClientRequest
// @see DMIndicator.h
// ----------------------------------------------------------------------------
bool DMIndicator::handleClientRequest(RequestType type,
        const QVariant &parameter)
    {
    qDebug("DMIndicator::handleClientRequest() start");
    bool handled(false);
    switch (type)
        {
        case RequestActivate:
            {
            qDebug("DMIndicator::handleClientRequest() RequestActivate");
            mParameter = parameter;
            emit
            dataChanged();
            handled = true;
            }
            break;
        case RequestDeactivate:
            {
            qDebug("DMIndicator::handleClientRequest() RequestDeactivate");
            emit deactivate();
            }
            break;
        default:
            break;
        }
    qDebug("DMIndicator::handleClientRequest() end");
    return handled;
    }
