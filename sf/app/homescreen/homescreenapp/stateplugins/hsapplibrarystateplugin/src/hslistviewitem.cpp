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
 * Description: List View Item.
 *
 */

#include <hbnamespace.h>
#include <HbAbstractItemView>
#include <HbStyleLoader>
#include <HbTextItem> 
#include <HbRichTextItem>
#include <HbParameterLengthLimiter>
#include <QPainter>
#include <caitemmodel.h>
#include "hsmenuitemmodel.h"

#include "hslistviewitem.h"


// TODO: this is only temporary class for show progress bar.
// It should be remove when fix from orbit will be in official platfrom.
// Remove it from header too.
// It is only about paint method.

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsProgressBar::paint(QPainter * painter,
        const QStyleOptionGraphicsItem * option,
        QWidget * widget)
{
    Q_UNUSED(widget)
    QStyleOptionGraphicsItem pixmapOption(*option);
    foreach (QGraphicsItem *child, childItems()) {
        painter->save();
        painter->translate(child->pos());
        pixmapOption.exposedRect = child->boundingRect();
        child->paint(painter, &pixmapOption, 0);

        foreach (QGraphicsItem *grandChild, child->childItems()) {
            if (grandChild->isVisible()) {
                painter->save();
                painter->translate(grandChild->pos());
                pixmapOption.exposedRect = grandChild->boundingRect();
                grandChild->paint(painter, &pixmapOption);
                painter->restore();
            }
        }
        painter->restore();
    }
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

void HsProgressBar::setTargetProgressValue(int value)
{
    mTargetValue = value;
    if (value > progressValue() && value <= maximum()) {
        if (!mTimerId) {
            mTimerId = startTimer(10);
        }
    } else {
        setProgressValue(value);
    }
}

void HsProgressBar::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if (mTargetValue == progressValue()) {
        killTimer(mTimerId);
        mTimerId = 0;
    } else {
        setProgressValue(progressValue()+1);
    }
 }

HsListViewItem::HsListViewItem(QGraphicsItem* parent) :
    HbListViewItem(parent), progress(0), isProgress(false)
{
    setGraphicsSize(LargeIcon);
    setStretchingStyle(StretchLandscape);
    if (this == prototype()) {
        HbStyleLoader::registerFilePath(":/layout/hslistviewitem.css");
    }
}

HsListViewItem::~HsListViewItem()
{
    if (this == prototype()) {
        HbStyleLoader::unregisterFilePath(":/layout/hslistviewitem.css");
        HbStyleLoader::unregisterFilePath(":/layout/hslistviewitem.widgetml");
    }
}

void HsListViewItem::updateChildItems()
{
    HSMENUTEST_FUNC_ENTRY("HsListViewItem::updateChildItems");
    HbListViewItem::updateChildItems();

    EntryFlags flags = modelIndex().data(
        CaItemModel::FlagsRole).value<EntryFlags> ();
    isProgress = false;
    if (flags & UninstallEntryFlag) {
        int progresVal = modelIndex().data(
                CaItemModel::UninstalRole).toInt();
        isProgress = true;
        if (!progress) {
            progress = new HsProgressBar(this);
            HbStyle::setItemName(progress, "progress");
            progress->setRange(0, 100);
            HbEffect::disable(progress);
            progress->setProgressValue(progresVal);
            repolish();
            connect(progress, SIGNAL(valueChanged(int)), SLOT(updatePixmapCache()));
        }
        progress->setTargetProgressValue(progresVal);
        // TODO, consider moving this logic to model
        foreach (QGraphicsItem * item, this->childItems()) {
               if (HbStyle::itemName(item) == "text-1") {
                   if (textFormat() == Qt::RichText) {
                       HbRichTextItem * text = (HbRichTextItem *)item;
                       text->setText(
                               HbParameterLengthLimiter("txt_applib_dblist_uninstalling_1")
                               .arg(text->text()));
                   }
                   else {
                       HbTextItem* text = (HbTextItem*)item;
                       text->setText(
                               HbParameterLengthLimiter("txt_applib_dblist_uninstalling_1")
                               .arg(text->text()));
                   }
                   break;
               }
           }
    } else if (progress) {
        disconnect(progress, SIGNAL(valueChanged()));
        delete progress;
        progress = 0;
        repolish();
    }
    // hide text-2 if we have to
    foreach (QGraphicsItem * item, this->childItems()) {
        if (HbStyle::itemName(item) == "text-2") {
            item->setVisible(!isProgress);
            break;
        }
    }
    HSMENUTEST_FUNC_EXIT("HsListViewItem::updateChildItems");
}

HbAbstractViewItem*  HsListViewItem::createItem()
{
    return new HsListViewItem(*this);
}


void HsListViewItem::polish(HbStyleParameters& params)
{
    HSMENUTEST_FUNC_ENTRY("HsListViewItem::polish");
    if (isProgress) {
        HbStyleLoader::registerFilePath(":/layout/hslistviewitem.widgetml");
    }
    HbListViewItem::setProperty("progress", isProgress);
    HbListViewItem::polish(params);
    if (isProgress) {
        HbStyleLoader::unregisterFilePath(":/layout/hslistviewitem.widgetml");
    }
    HSMENUTEST_FUNC_EXIT("HsListViewItem::polish");
}

