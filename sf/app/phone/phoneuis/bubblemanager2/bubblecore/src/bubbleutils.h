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

#ifndef BUBBLEUTILS_H_
#define BUBBLEUTILS_H_

#include <QString>

class BubbleHeader;
class BubbleCallIndicator;
class HbIconItem;
class BubbleButton;
class HbAction;
class HbTextItem;

class BubbleUtils
    {
public:
    static void setCallHeaderTexts3Lines(
        const BubbleHeader& header,
        HbTextItem& textLine1,
        HbTextItem& textLine2,
        HbTextItem& textLine3,
        int& cliLineNumber,
        int& timerLineNumber);

    static void setCallHeaderTexts2Lines(
        const BubbleHeader& header,
        HbTextItem& textLine1,
        HbTextItem& textLine2,
        int& cliLineNumber,
        int& timerLineNumber);

    static bool compareHeaders(
        const BubbleHeader* header1,
        const BubbleHeader* header2);

    static void setButtonStyleForAction(
        BubbleButton& button,
        const HbAction& action);

    static void setIndicators(
        int callState,
        int callFlags,
        HbIconItem& indicator1,
        HbIconItem& indicator2);

private:
    static void setText(
        HbTextItem& item,
        const QString& text,
        Qt::TextElideMode clip);

    };

#endif /* BUBBLEUTILS_H_ */
