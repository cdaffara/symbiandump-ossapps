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
* Description: Bubble heading widget
*
*/

#ifndef BUBBLEHEADINGWIDGET_H
#define BUBBLEHEADINGWIDGET_H

#include <hbwidget.h>

class HbIconItem;
class HbTextItem;
class HbFontSpec;
class BubbleHeader;

class BubbleHeadingWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(int lineCount READ lineCount WRITE setLineCount)
    Q_PROPERTY(QString layoutOption READ layoutOption WRITE setLayoutOption)
    Q_PROPERTY(QString textLine1 READ textLine1)
    Q_PROPERTY(QString textLine2 READ textLine2)
    Q_PROPERTY(QString textLine3 READ textLine3)
    Q_PROPERTY(QString indicator1 READ indicator1)
    Q_PROPERTY(QString indicator2 READ indicator2)

public:
    BubbleHeadingWidget(QGraphicsItem* item=0);
    virtual ~BubbleHeadingWidget();

    int lineCount() const;
    void setLineCount(int count);

    QString layoutOption() const;
    void setLayoutOption(const QString& option);

    QString textLine1() const;

    QString textLine2() const;

    QString textLine3() const;

    QString indicator1() const;

    QString indicator2() const;

    void reset();
    void readBubbleHeader(const BubbleHeader& header);
    void updateTimerDisplayNow();

private:
    void createPrimitives();
    void updatePrimitives();

protected:
    void polish(HbStyleParameters &params);

private:
    int mLineCount;
    QString mLayoutOption;

    const BubbleHeader* mHeader;

    // primitives
    HbIconItem* mIndicator1;
    HbIconItem* mIndicator2;
    HbTextItem* mText1;
    HbTextItem* mText2;
    HbTextItem* mText3;
    int mCallTimerTextLine;
    HbFontSpec* mCliFont;
    HbFontSpec* mTextFont;
};

#endif // BUBBLEHEADINGWIDGET_H
