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
 * Description: DM Progress and Notification indicators implementation
 *
 */

#include <QByteArray>
#include <QVariant>
#include <e32base.h>
#include <w32std.h>
#include <apgtask.h>
#include <apgtask.h>
#include <hbindicator.h>
#include <eikenv.h>
#include "dmindicator.h" 
#include "dmpronoteindicators.h" 

// ----------------------------------------------------------------------------
// DMProNoteIndicator::DMProNoteIndicator
// @see dmpronoteindicators.h
// ----------------------------------------------------------------------------
DMProNoteIndicator::DMProNoteIndicator(const QString &indicatorType,
        HbIndicatorInterface::Category category,
        HbIndicatorInterface::InteractionTypes interactionTypes) :
    DMIndicator(indicatorType, category, interactionTypes)
    {

    }

// ----------------------------------------------------------------------------
// DMProNoteIndicator::indicatorData
// @see dmpronoteindicators.h
// ----------------------------------------------------------------------------
QVariant DMProNoteIndicator::indicatorData(int role) const
    {
    qDebug("DMProNoteIndicator::indicatorData()");
    switch (role)
        {
        case PrimaryTextRole:
            {
            QString text = hbTrId("txt_device_update_dblist_device_updates");
            return text;
            }
        case SecondaryTextRole:
            {
            QString text = mParameter.toString();
            return text;
            }
        case MonoDecorationNameRole:
            {
            //QString iconName("C:/resource/devicemanagement/qtg_small_smiley_kissing.svg");
            QString iconName("qtg_small_smiley_kissing");
            return iconName;
            }

        case DecorationNameRole:
            {
            //QString iconName("C:/resource/devicemanagement/qtg_small_smiley_kissing.svg");
            QString iconName("qtg_small_smiley_kissing");
            return iconName;
            }
        default:
            qDebug(
                    "DMProNoteIndicator::indicatorData() returning Null QVariant");
            return QVariant();

        }
    }

// ----------------------------------------------------------------------------
// DMProNoteIndicator::handleInteraction
// @see dmpronoteindicators.h
// ----------------------------------------------------------------------------
bool DMProNoteIndicator::handleInteraction(InteractionType type)
    {
    qDebug("DMProNoteIndicator::handleInteraction() Start");
	QVariantMap map;
    map.insert("data", indicatorType());
    emit userActivated(map);
    emit deactivate();
    qDebug("DMProNoteIndicator::handleInteraction() End");
    return true;
    }

// ----------------------------------------------------------------------------
// ScomoProNoteIndicator::ScomoProNoteIndicator
// @see dmpronoteindicators.h
// ----------------------------------------------------------------------------
ScomoProNoteIndicator::ScomoProNoteIndicator(const QString &indicatorType,
        HbIndicatorInterface::Category category,
        HbIndicatorInterface::InteractionTypes interactionTypes) :
DMProNoteIndicator(indicatorType, category, interactionTypes)
    {

    }

// ----------------------------------------------------------------------------
// ScomoProNoteIndicator::handleInteraction
// @see dmpronoteindicators.h
// ----------------------------------------------------------------------------
bool ScomoProNoteIndicator::handleInteraction(InteractionType type)
    {
    qDebug("ScomoProNoteIndicator::handleInteraction() Start");
    bool handled = false;
    
    emit deactivate();
    CCoeEnv* coe = CCoeEnv::Static(); 
    TApaTaskList taskList(coe->WsSession());
    TApaTask task=taskList.FindApp(TUid::Uid(0x200267FB));
    if(task.Exists())
        {
        task.BringToForeground();
        }
    qDebug("ScomoProNoteIndicator::handleInteraction() End");
    return handled;
    }


