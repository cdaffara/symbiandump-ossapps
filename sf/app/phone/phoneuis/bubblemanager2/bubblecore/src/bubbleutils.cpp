/*!
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
* Description:  Utility methods.
*
*/

#include <hbiconitem.h>
#include <hbaction.h>
#include <hbtextitem.h>
#include <hbstringutil.h>
#include <hbiconanimator.h>
#include <hbcolorscheme.h>

#include "bubblemanager2.h"
#include "bubbleutils.h"
#include "bubbleheader.h"
#include "bubblebutton.h"

void BubbleUtils::setCallHeaderTexts3Lines(
    const BubbleHeader& header,
    HbTextItem& textLine1,
    HbTextItem& textLine2,
    HbTextItem& textLine3,
    int& cliLineNumber,
    int& timerLineNumber)
{
    switch( header.callState() ) {
    case BubbleManager::Incoming:
    case BubbleManager::Waiting:
    case BubbleManager::AlertToDisconnected:
    {
        if ( header.secondaryCli().length() ) {
            setText(textLine1, header.cli(), header.cliClipDirection());
            setText(textLine2, header.secondaryCli(),
                    header.secondaryCliClipDirection());
            setText(textLine3, header.text(), header.textClipDirection());
            cliLineNumber = 1;
        } else {
            setText(textLine1,header.cli(), header.cliClipDirection());
            setText(textLine2,header.text(), header.textClipDirection());
            cliLineNumber = 1;
        }
        break;
    }

    case BubbleManager::Outgoing:
    {
        if ( header.secondaryCli().length() ) {
            setText(textLine1, header.text(), header.textClipDirection());
            setText(textLine2, header.cli(), header.cliClipDirection());
            setText(textLine3, header.secondaryCli(),
                    header.secondaryCliClipDirection());
            cliLineNumber = 2;
        } else {
            setText(textLine1, header.text(), header.textClipDirection());
            setText(textLine2, header.cli(), header.cliClipDirection());
            cliLineNumber = 2;
        }
        break;
    }

    case BubbleManager::Active:
    case BubbleManager::Alerting:
    {
        if ( header.secondaryCli().length() ) {
            setText(textLine1, header.cli(), header.cliClipDirection());
            setText(textLine2, header.secondaryCli(),
                    header.secondaryCliClipDirection());
            setText(textLine3, header.timerCost(), Qt::ElideRight);
            cliLineNumber = 1;
            timerLineNumber = 3;
        } else {
            setText(textLine1, header.cli(), header.cliClipDirection());
            setText(textLine2, header.timerCost(), Qt::ElideRight);
            cliLineNumber = 1;
            timerLineNumber = 2;
        }
        break;
    }

    case BubbleManager::OnHold:
    {
        if ( header.secondaryCli().length() ) {
            setText(textLine1, header.cli(), header.cliClipDirection());
            setText(textLine2, header.secondaryCli(),
                    header.secondaryCliClipDirection());
            setText(textLine3, header.text(), header.textClipDirection());
            cliLineNumber = 1;
        } else {
            setText(textLine1, header.cli(), header.cliClipDirection());
            setText(textLine2, header.text(), header.textClipDirection());
            cliLineNumber = 1;
        }
        break;
    }

    case BubbleManager::Disconnected:
    {
        if ( header.timerCost().length() ) {
            setText(textLine1, header.cli(), header.cliClipDirection());
            setText(textLine2, header.text(), header.textClipDirection());
            setText(textLine3, header.timerCost(), Qt::ElideRight);
            cliLineNumber = 1;
            timerLineNumber = 3;
        } else {
            if ( header.secondaryCli().length() ) {
                setText(textLine1, header.cli(), header.cliClipDirection());
                setText(textLine2, header.secondaryCli(),
                        header.secondaryCliClipDirection());
                setText(textLine3, header.text(), header.textClipDirection());
                cliLineNumber = 1;
            } else {
                setText(textLine1, header.cli(), header.cliClipDirection());
                setText(textLine2, header.text(), header.textClipDirection());
                cliLineNumber = 1;
            }
        }
        break;
    }


    default:
        // do nothing
        break;
    } // switch
}

void BubbleUtils::setCallHeaderTexts2Lines(
    const BubbleHeader& header,
    HbTextItem& textLine1,
    HbTextItem& textLine2,
    int& cliLineNumber,
    int& timerLineNumber)
{
    switch( header.callState() ) {
    case BubbleManager::Active:
        setText(textLine1, header.cli(), header.cliClipDirection());
        setText(textLine2, header.timerCost(), Qt::ElideRight);
        cliLineNumber = 1;
        timerLineNumber = 2;
        break;


    case BubbleManager::OnHold:
    case BubbleManager::Disconnected:
        setText(textLine1, header.cli(), header.cliClipDirection());
        setText(textLine2, header.text(), header.textClipDirection());
        cliLineNumber = 1;
        break;

    case BubbleManager::Waiting:
    case BubbleManager::AlertToDisconnected:
        setText(textLine1, header.cli(), header.cliClipDirection());
        setText(textLine2, header.text(), header.textClipDirection());
        cliLineNumber = 1;
        break;

    case BubbleManager::Outgoing:
        setText(textLine1, header.text(), header.textClipDirection());
        setText(textLine2, header.cli(), header.cliClipDirection());
        cliLineNumber = 2;
        break;

    default:
        // do nothing
        break;
    } // switch
}

void BubbleUtils::setText(
    HbTextItem& item,
    const QString& text,
    Qt::TextElideMode clip)
{
    if (clip == Qt::ElideLeft) {
        // convert phonenumber to phone ui language
        QString converted = HbStringUtil::convertDigits(text);
        item.setText(converted);
    } else {
        item.setText( text );
    }
    item.setElideMode( clip );
}

bool BubbleUtils::compareHeaders(
    const BubbleHeader* header1,
    const BubbleHeader* header2 )
{
    // sort according to call states, but keep conference header
    // last, it shown at foreground only when expanded.
    if ( (header1->callState() < header2->callState()) ||
         header1->isConference() ) {
        return false;
    } else {
        return true;
    }
}

void BubbleUtils::setButtonStyleForAction(
    BubbleButton& button,
    const HbAction& action)
{
    if (action.softKeyRole()==QAction::PositiveSoftKey) {
        button.setButtonType(BubbleButton::GreenButton);
    } else if (action.softKeyRole()==QAction::NegativeSoftKey) {
        button.setButtonType(BubbleButton::RedButton);
    } else {
        button.setButtonType(BubbleButton::DefaultButton);
    }

    button.updatePrimitives();
}

void BubbleUtils::setIndicators(
    int callState,
    int callFlags,
    HbIconItem& indicator1,
    HbIconItem& indicator2 )
{
    bool divertIndicator = (( callState == BubbleManagerIF::Incoming ) ||
                            ( callState == BubbleManagerIF::Waiting )) &&
                           (callFlags & BubbleManagerIF::Diverted);

    bool cipheringIndicator = (callFlags & BubbleManagerIF::NoCiphering);

    if (divertIndicator && cipheringIndicator) {
        indicator1.setIcon(HbIcon(QLatin1String("qtg_mono_ciphering_off")));
        indicator1.show();
        indicator2.setIcon(HbIcon(QLatin1String("qtg_mono_call_diverted")));
        indicator2.show();
    } else if (cipheringIndicator) {
        indicator1.setIcon(HbIcon(QLatin1String("qtg_mono_ciphering_off")));
        indicator1.show();
        indicator2.setIcon(HbIcon());
        indicator2.hide();
    } else if (divertIndicator) {
        indicator1.setIcon(HbIcon(QLatin1String("qtg_mono_call_diverted")));
        indicator1.show();
        indicator2.setIcon(HbIcon());
        indicator2.hide();
    } else {
        indicator1.setIcon(HbIcon());
        indicator1.hide();
        indicator2.setIcon(HbIcon());
        indicator2.hide();
    }
}
