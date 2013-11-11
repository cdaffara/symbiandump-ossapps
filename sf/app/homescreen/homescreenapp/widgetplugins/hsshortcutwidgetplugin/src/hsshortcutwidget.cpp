/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Home screen shortcut widget.
*
*/

#include <QGraphicsSceneMouseEvent>

#include <HbStyleLoader>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <HbTextItem>
#include <HbInstantFeedback>
#include <HbTapGesture>

#include "hsshortcutwidget.h"
#include "hsshortcutservice.h"
#include "hsapp_defs.h"
#include "caservice.h"
#include "canotifier.h"
#include "caquery.h"
#include "hsconfiguration.h"

/*!
    \class HsShortcutWidget
    \ingroup group_hsshortcutwidgetplugin
    \brief Implementation for the homescreen shortcut widget.

    Shortcut can be defined to launch different applications or applications
    with parameters, for example browser with certain url.
*/

/*!
    Constructor.
*/
HsShortcutWidget::HsShortcutWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
  : HbWidget(parent, flags),
    mBackground(0), mIcon(0), mText(0),
    mCaEntryId(-1), mCaEntryRole(ItemEntryRole)
{
    grabGesture(Qt::TapGesture);

    HbStyleLoader::registerFilePath(":/hsshortcutwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsshortcutwidget.css");

    createPrimitives();
}

/*!
    Destructor.
*/
HsShortcutWidget::~HsShortcutWidget()
{
    HbStyleLoader::unregisterFilePath(":/hsshortcutwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/hsshortcutwidget.css");
}

/*!
    Sets the menu content service id.
*/
void HsShortcutWidget::setCaEntryId(int caEntryId)
{
    mCaEntryId = caEntryId;
}

/*!
    Returns the menu content service id.
*/
int HsShortcutWidget::caEntryId() const
{
    return mCaEntryId;
}

/*!
    Sets the application's UID this shortcut points to.
*/
void HsShortcutWidget::setUid(const QString &uid)
{
    mUid = uid;
}

/*!
    Returns the application's UID this shortcut points to.
*/
QString HsShortcutWidget::uid() const
{
    return mUid;
}

/*!
    Returns the text property. This property is needed by css selector.
*/
QString HsShortcutWidget::text() const
{
    if ( mText ) {
        return mText->text();
    } else {
        return QString();
        }
}

/*!
    Sets the text property. This property is needed by css selector.
*/
void HsShortcutWidget::setText(const QString& textItem)
{
    if ( mText ) {
        mText->setText(textItem);
    }
}

/*!
    Initializes this widget.
*/
void HsShortcutWidget::onInitialize()
{
    QSharedPointer<CaEntry> caEntry;

    if(-1 == mCaEntryId) {
        //Shortcut can be pre installed to Homescreen's DB by setting
        //application UID for its preference. Entry Id is retrieved
        //from CA by UID.
        CaQuery query;
        query.setEntryTypeNames(QStringList(Hs::applicationTypeName));
        //Convert UID from hex to ten base, because CA t UIDs in ten base.
        bool ok;
        int hexBaseInteger = mUid.toInt(&ok, 0); // Qt Assistant: If base is 0, the C language convention is used:
                                             // If the string begins with "0x", base 16 is used.
        QString tenBaseString = QString::number(hexBaseInteger);
        query.setAttribute(Hs::applicationUidEntryKey, tenBaseString);
        QList< QSharedPointer<CaEntry> > appEntries = CaService::instance()->getEntries(query);

        //Verify that entry's UID is what we want since we get all application entries
        //if UID did not match in getEntries() above.
        if (!appEntries.isEmpty()
            && appEntries.first()->attribute(Hs::applicationUidEntryKey) == tenBaseString) {
            caEntry = appEntries.first();
            mCaEntryId = caEntry->id();
			//Save caEntryId to Homescreen database
            emit setPreferences(QStringList() << QLatin1String("caEntryId"));
        }
    } else {
        caEntry = CaService::instance()->getEntry(mCaEntryId);
    }

    if (!caEntry.isNull()) {
        createCaNotifier();
        updateContent(*caEntry.data());
    } else {
        emit finished();
    }
}

/*!
    Wakes up this widget.
*/
void HsShortcutWidget::onShow()
{
    updateVisibility();
}

/*!
    Puts this widget in quiescent state.
*/
void HsShortcutWidget::onHide()
{
}

/*!
    \internal  
*/
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsShortcutWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture));
    if (gesture) {
        switch (gesture->state()) {
            case Qt::GestureStarted:
                setBackgroundToPressed();
                break;            
            case Qt::GestureCanceled:
                setBackgroundToNormal();
                break;
            case Qt::GestureFinished:
                setBackgroundToNormal();
                if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                    launch();
                }
                break;
            default:
                break;
        }
    }
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    \internal
*/
void HsShortcutWidget::launch()
{
    HbInstantFeedback::play(HSCONFIGURATION_GET(shortcutWidgetTapFeedbackEffect));
    
    if (mCaEntryRole == ItemEntryRole) {
        CaService::instance()->executeCommand(mCaEntryId);
    } else {
        HsShortcutService::instance()->executeCollectionAction(mCaEntryId, mCaEntryTypeName);
    }
}

/*!
    \internal
*/
void HsShortcutWidget::createPrimitives()
{
    // Background
    if (!mBackground) {
        HbFrameDrawer *drawer = new HbFrameDrawer(
            QLatin1String("qtg_fr_hsshortcut_normal"), HbFrameDrawer::NinePieces);
        mBackground = new HbFrameItem(drawer, this);
        HbStyle::setItemName(mBackground, QLatin1String("background"));
    }

    // Icon
    if (!mIcon) {
        mIcon = new HbIconItem(this);
        HbStyle::setItemName(mIcon, QLatin1String("icon"));
    }

    // Text
    if (HSCONFIGURATION_GET(isShortcutLabelVisible) && !mText ) {
        mText = new HbTextItem(this);
        HbStyle::setItemName(mText, QLatin1String("text"));
    }
}

/*!
    \internal
*/
void HsShortcutWidget::updateContent(const CaEntry &caEntry)
{
    mCaEntryRole = caEntry.role();
    mCaEntryFlags = caEntry.flags();
    mCaEntryTypeName = caEntry.entryTypeName();
    mIcon->setIcon(caEntry.makeIcon());
    if (mText) {
        if(caEntry.attribute(Hs::entryShortName).length()) {
            mText->setText(caEntry.attribute(Hs::entryShortName));
        } else {
            mText->setText(caEntry.text());
        }        
    }
}

/*!
    \internal
*/
void HsShortcutWidget::updateVisibility()
{
    setVisible(!mCaEntryFlags.testFlag(MissingEntryFlag));
}

/*!
    \internal
*/
void HsShortcutWidget::setBackgroundToNormal()
{
    mBackground->frameDrawer().
        setFrameGraphicsName(QLatin1String("qtg_fr_hsshortcut_normal"));
}

/*!
    \internal
*/
void HsShortcutWidget::setBackgroundToPressed()
{
    mBackground->frameDrawer().
        setFrameGraphicsName(QLatin1String("qtg_fr_hsitems_pressed"));
}

/*!
    \internal
*/
void HsShortcutWidget::createCaNotifier()
{
    CaNotifierFilter filter;
    filter.setIds(QList<int>() << mCaEntryId);

    CaNotifier *notifier = CaService::instance()->createNotifier(filter);
    notifier->setParent(this);

    connect(notifier,
        SIGNAL(entryChanged(CaEntry,ChangeType)),
        SLOT(onEntryChanged(CaEntry,ChangeType)),
        Qt::QueuedConnection);
}

/*!
    \internal
*/
void HsShortcutWidget::onEntryChanged(const CaEntry &caEntry, ChangeType changeType)
{
    updateContent(caEntry);
    updateVisibility();

    if ( changeType == RemoveChangeType && !(caEntry.flags() & MissingEntryFlag)) {
        //genuinely removed
        emit finished();
    }
}
