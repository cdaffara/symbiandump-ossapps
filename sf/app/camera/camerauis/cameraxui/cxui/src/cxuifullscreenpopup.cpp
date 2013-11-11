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

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsAnchorLayout>
#include <HbLabel>
#include <HbPushButton>
#include <HbInstantFeedback>
#include "cxutils.h"
#include "cxuienums.h"
#include "cxuiview.h"
#include "cxuidocumentloader.h"
#include "cxuifullscreenpopup.h"

/*!
* Constructor.
*/
CxuiFullScreenPopup::CxuiFullScreenPopupLoader::CxuiFullScreenPopupLoader()
    : HbDocumentLoader()
{
}

/*!
* Create object from DocML.
* Custom type of object created when object name matches the full screen dialog one.
* Other items are propagated to HbDocumentLoader for standard handling.
* @param type Object type (class name)
* @param name Object name, name of the item in DocML.
*/
QObject *CxuiFullScreenPopup::CxuiFullScreenPopupLoader::createObject(const QString& type, const QString &name)
{
    CX_DEBUG_ENTER_FUNCTION();
    QObject *object = NULL;

    // HbWidget as type in DocML to support UI layout tool.
    if (type == "HbWidget") {
        if (name == CxUiLayout::FULL_SCREEN_POPUP) {
            object = new CxuiFullScreenPopup();
        }
    }

    if (object) {
        object->setObjectName(name);
    } else {
        object = HbDocumentLoader::createObject(type, name);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return object;
}

/*!
* Load a full screen popup from DocML and set given message and action text to it.
* @param scene Graphics scene where to insert the popup.
* @param message Popup message text.
* @param action Popup action button text.
*/
CxuiFullScreenPopup *CxuiFullScreenPopup::create(QGraphicsScene *scene,
                                                 const QString &message,
                                                 const QString &action)
{
    CX_DEBUG_ENTER_FUNCTION();

    bool ok(true);

    // Load popup structure from DocML.
    CxuiFullScreenPopupLoader *loader = new CxuiFullScreenPopupLoader();
    loader->load(CxUiLayout::FULL_SCREEN_POPUP_XML, &ok);
    CX_ASSERT_ALWAYS(ok);

    // Popup container widget.
    CxuiFullScreenPopup *popup = qobject_cast<CxuiFullScreenPopup *>(loader->findWidget(CxUiLayout::FULL_SCREEN_POPUP));
    CX_ASSERT_ALWAYS(popup);
    // Popup needs scene to move in front of all widgets.
    popup->setScene(scene);
    // Add background graphic for popup.
    QGraphicsRectItem *bg = new QGraphicsRectItem(popup);
    bg->setRect(popup->geometry());
    bg->setBrush(Qt::black);
    popup->setBackgroundItem(bg);

    // Popup text.
    //!@todo: Move text color definition to DocML once supported there.
    HbLabel *text = qobject_cast<HbLabel*>(loader->findWidget(CxUiLayout::FULL_SCREEN_POPUP_TEXT));
    CX_ASSERT_ALWAYS(text);
    text->setPlainText(message);
    text->setTextColor(Qt::white);

    // Action button.
    // Hidden if empty text given.
    HbPushButton *button = qobject_cast<HbPushButton*>(loader->findWidget(CxUiLayout::FULL_SCREEN_POPUP_BUTTON));
    CX_ASSERT_ALWAYS(button);
    if (action.isEmpty()) {
        CX_DEBUG(("CxuiFullScreenPopup - no button text given, hiding button.."));
        button->setVisible(false);
    } else {
        button->setText(action);
    }

    delete loader;
    loader = NULL;

    CX_DEBUG_EXIT_FUNCTION();
    return popup;
}

/*!
* Popup destructor.
*/
CxuiFullScreenPopup::~CxuiFullScreenPopup()
{
    CX_DEBUG_ENTER_FUNCTION();
    hide();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle mouse press event.
*/
void CxuiFullScreenPopup::handleMousePress()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Give standard feedback.
    HbInstantFeedback feedback(HbFeedback::BasicItem);
    feedback.setModalities(HbFeedback::All);
    feedback.play();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle mouse release event.
*/
void CxuiFullScreenPopup::handleMouseRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    //!@todo: sound disabling doesn't work in orbit yet so don't do feedback on release
    // needs to be enabled when orbit support is done
    // Give tactile feedback but no sound feedback on mouse release.
    /*
    HbInstantFeedback feedback(HbFeedback::BasicItem);
    feedback.setModalities(HbFeedback::Tactile);
    feedback.play();
    CX_DEBUG_EXIT_FUNCTION();
    */
}

/*!
* Show the popup.
* Checks current top most item in the scene and adds this popup in front of it.
* Input to below widgets is blocked.
*/
void CxuiFullScreenPopup::show()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_ASSERT_ALWAYS(mScene);

    if (!mPopupShown) {
        mPopupShown = true;
        // Store current top item z value.
        qreal topZ(topZValue());
        // Add popup widget to scene to actually get it visible.
        mScene->addItem(this);
        // Make sure popup is just above the top item.
        setZValue(topZ + 1);

        // Block input to other widgets below the popup.
        setWindowFlags(Qt::Dialog);
        setPanelModality(QGraphicsItem::PanelModal);

        // Show popup.
        HbWidget::show();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Hide the popup.
*/
void CxuiFullScreenPopup::hide()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mPopupShown) {
        HbWidget::hide();
        // Double check non-nullness.
        if (mScene) {
            mScene->removeItem(this);
        }
        mPopupShown = false;
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get the z value of top item in scene.
* @return Top (max) z value.
*/
qreal CxuiFullScreenPopup::topZValue() const
{
    CX_DEBUG_ENTER_FUNCTION();

    qreal z(0);
    if (mScene) {
        const QList<QGraphicsItem *> itemList(mScene->items(Qt::DescendingOrder));
        foreach (QGraphicsItem *item, itemList) {
            // Max of z values.
            z = (item && item->zValue() > z) ? item->zValue() : z;
#ifdef CX_DEBUG
            const QGraphicsObject *gObject(item->toGraphicsObject());
            CX_DEBUG(("Item of class [%s] z value [%s]",
                      gObject ? gObject->metaObject()->className() : qPrintable(QString("QGraphicsItem")),
                      qPrintable(QString::number(item->zValue()))));
#endif // CX_DEBUG
        }
    } else {
        CX_DEBUG(("[WARNING] No scene found, returning zero"));
    }
    CX_DEBUG_EXIT_FUNCTION();
    return z;
}

/*!
* Store the scene in which this popup is to be shown.
*/
void CxuiFullScreenPopup::setScene(QGraphicsScene *scene)
{
    mScene = scene;
}

/*!
* Popup (private) constructor.
*/
CxuiFullScreenPopup::CxuiFullScreenPopup()
    : mScene(NULL),
      mPopupShown(false)
{
    CX_DEBUG_IN_FUNCTION();
}

// end of file
