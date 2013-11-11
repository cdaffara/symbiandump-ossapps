/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxutils.h"
#include "cxeengine.h"
#include "cxuisettingsinfo.h"
#include "cxesettings.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuisettingradiobuttonlistmodel.h"

CxuiSettingRadioButtonList::CxuiSettingRadioButtonList(QGraphicsItem *parent, CxeEngine *engine)
: HbRadioButtonList(parent),
  mEngine(engine),
  mSettingId(),
  mSettingValues(),
  mOriginalIndex(0)
{
    CX_ASSERT_ALWAYS(mEngine);

    mListModel = new CxuiSettingRadioButtonListModel();
    setModel(mListModel);

    // connect the signals again
    connect(this, SIGNAL(itemSelected(int)), this, SLOT(handleItemSelected(int)));
}

/*!
* Init contents of the listbox and select current setting value.
*/
void CxuiSettingRadioButtonList::init(CxUiSettings::RadioButtonListParams *data)
{
    // first we reset the model and clear any previous data
    mSettingValues.clear();
    mListModel->resetModel();

    disconnect(SIGNAL(valueSelected(int)));

    if (data) {
        QStringList settingStrings;
        mSettingValues.clear();

        CxUiSettings::SettingItem setting;
        foreach (setting, data->mSettingPairList) {
            CX_DEBUG(("CxuiSettingRadioButtonList - appending setting value: %s", setting.mValue.toString().toAscii().data()));
            settingStrings.append(setting.mItem); // setting string
            mSettingValues.append(setting.mValue); // engine value for setting
        }

        setListBoxType(data->mListboxType);
        // Set the setting strings to the model.
        setItems(settingStrings);
        // Set the preview mode.
        // Note: We implement preview ourselves, not with HbRadioButtonList preview mode.
        setPreviewMode(HbRadioButtonList::NoPreview);
        mPreview = data->mPreview;

        setSettingId(data->mSettingId);

        // Store the original setting value and focus matching item.
        QString value = mEngine->settings().get<QString>(mSettingId, "");
        CX_DEBUG(("CxuiSettingRadioButtonList - original value: [%s]", qPrintable(value)));
        setOriginalSelectedItemByValue(QVariant(value));
    }
}

/*!
 * Sets the original selection of list by value. Can be used to override value read from
 * CxeSettings or used to remember previously selected value in case of setting that is
 * not read from CxeSettings (e.g. selftimer)
 */
void CxuiSettingRadioButtonList::setOriginalSelectedItemByValue(const QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Find the index of given value among setting values.
    // Default to first item, if given value is not found.
    int index = mSettingValues.indexOf(QVariant(value));
    if (index < 0) {
        CX_DEBUG(("[WARNING] Value [%s] not found, selecting first item", qPrintable(value.toString())));
        index = 0;
    }

    // Store the original value.
    mOriginalIndex = index;
    // Select the index with current value item.
    setSelected(index);
    // Ensure that currently selected item is visible.
    scrollTo(currentIndex());

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set list texts.
* @param values List of the texts.
*/
void CxuiSettingRadioButtonList::setItems(const QStringList &values)
{
    mListModel->setItems(values);
}

/*!
* Set the type of this list.
* @param type Type identifier, SingleLine or TwoLine.
*/
void CxuiSettingRadioButtonList::setListBoxType(int type)
{
    mListModel->setListBoxType(type);
}

/*!
* Set id of the setting this list represents.
* @param id Id of the setting.
*/
void CxuiSettingRadioButtonList::setSettingId(const QString &id)
{
    // Selected item is updated, when this list is shown.
    mSettingId = id;
}

/*!
* Handle selecting an item.
* @param index Index of the selected item in list.
*/
void CxuiSettingRadioButtonList::handleItemSelected(int index)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mPreview) {
        commit(index);
    } else {
        // no action needed
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    This slot can be used to set the selection accepted.
*/
void CxuiSettingRadioButtonList::handleSelectionAccepted()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ok button pressed. Update originally selected item,
    // which is the one that always get's set when closing.
    mOriginalIndex = selected();

    emit selectionCommitted();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle closing the listbox. If the current selection was accepted,
* we commit the new value here. If current selection has been cancelled,
* we commit the original value.
*/
void CxuiSettingRadioButtonList::handleClose()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mSettingId.isEmpty()) {
        // If handleSelectionAccepted was called, this now contains the new index,
        // otherwise the original one is selected.
        commit(mOriginalIndex);
    }

    // clear settings id so setting value doesn't get updated by
    // accident when updating ui appearance
    mSettingId.clear();

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
  Commits value to settings.
*/
void CxuiSettingRadioButtonList::commit(int index)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxuiSettingRadioButtonList - id: %s", qPrintable(mSettingId)));

    if (!mSettingId.isEmpty() && !mSettingValues.isEmpty()) {
        QVariant value = mSettingValues.at(index);
        if (value.type() == QVariant::Int) {
            CX_DEBUG(("CxuiSettingRadioButtonList - index:%d value:%d", index, value.toInt()));

            // Don't set the value again, if it is the current value.
            // For e.g. video quality it would result in re-preparation etc.
            try {
                int current = mEngine->settings().get<int>(mSettingId);
                if (current != value.toInt()) {
                    mEngine->settings().set(mSettingId, value.toInt());
                }
            } catch (CxeException &e) {
                // ignore error
            }
            // inform interested clients about value changed event
            emit valueSelected(value.toInt());

        } else if (value.type() == QVariant::String) {
            CX_DEBUG(("CxuiSettingRadioButtonList - index:%d value:[%s]", index, qPrintable(value.toString())));

            try {
                QString current = mEngine->settings().get<QString>(mSettingId);
                CX_DEBUG(("CxuiSettingRadioButtonList - settings model value:[%s]", qPrintable(current)));
                // If the changed setting is a scene mode, we need to reset it
                // because a scene mode related setting might have changed
                // even though the scene mode itself hadn't
                if (current != value.toString() ||
                    mSettingId == CxeSettingIds::IMAGE_SCENE ||
                    mSettingId == CxeSettingIds::VIDEO_SCENE) {
                    mEngine->settings().set(mSettingId, value.toString());
                }
            } catch (CxeException &e) {
                // ignore error
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
