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

#include "cxuisettingslider.h"
#include "cxeengine.h"
#include "cxesettings.h"
#include "cxutils.h"

CxuiSettingSlider::CxuiSettingSlider(QGraphicsItem *parent, CxeEngine *engine) :
HbSlider(parent),
mSettingId(),
mEngine(engine),
mSettings(NULL),
mOriginalValue(0),
mSliderScaleValue(1)
{
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(handleValueChanged(int)));
}

/*!
    Initialises slider according to SliderParams data
*/
void CxuiSettingSlider::init(CxUiSettings::SliderParams *data)
{
    CX_DEBUG_ENTER_FUNCTION();

    // initialise engine side settings
    if (mEngine) {
        mSettings = &mEngine->settings();
        CX_ASSERT_ALWAYS(mSettings);
    }

    // initialise data
    if (data) {

        // scale value is needed because slider doesn't show all ticks and
        // labels correctly if step is something else than 1
        int step = 1;
        mSliderScaleValue = 1;
        if (data->mMinorStep != 0) {
            mSliderScaleValue = step / data->mMinorStep;
        }

        // scale min and max values as step has been set to 1
        int minvalue = data->mRange.first * mSliderScaleValue;
        int maxvalue = data->mRange.second * mSliderScaleValue;
        CX_DEBUG(("Setting slider range [%d..%d]", minvalue, maxvalue));
        setRange(minvalue,maxvalue);
        setSingleStep(step);
         
        setSnappingMode(HbSlider::MinorTickSnapping);

        setMajorTickLabels(data->mSettingStrings);
        setMajorTickInterval(mSliderScaleValue * data->mMajorStep);
        setMinorTickInterval(mSliderScaleValue * data->mMinorStep);
        setTickPosition(Hb::SliderTicksAbove);

        setSettingId(data->mSettingId);

    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Returns settingId
*/
QString CxuiSettingSlider::settingId() const
{
    return mSettingId;
}

/*!
    Sets the setting identification string. Setting id corresponds to specific setting
    defined in cxenamespace.h. A value that corresponds the id is got from engine and
    stored as original value. Also slider value is updated.
*/
void CxuiSettingSlider::setSettingId(const QString &id)
{
    CX_DEBUG_ENTER_FUNCTION();

    mSettingId = id;

    // once the settingid is set, we can get the current value for the slider from the engine
    qreal value = mSettings->get<qreal>(mSettingId, 0);

    // engine value has to be scaled when set to slider
    setValue(value * mSliderScaleValue);
    mOriginalValue = value;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    This slot is called when slider's value is changed.
    Value is stored to engine.
*/
void CxuiSettingSlider::handleValueChanged(int value)
{
    CX_DEBUG_ENTER_FUNCTION();

    qreal realValue = 0.0;
    if (mSliderScaleValue != 0) {
        realValue = value / mSliderScaleValue;
    }

    CX_DEBUG(("id: %s", mSettingId.toAscii().data()));
    CX_DEBUG(("changing value to: %f ", realValue));

    // check added because when the slider is created (or ui appearance changed)
    // handlevaluechanged function is called before the mSettingId is set
    if (!mSettingId.isEmpty()){
        mSettings->set(mSettingId, realValue);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    This slot should be called when slider is closed. Original value will always be given
    to engine. Original value is the value that slider had when it was initialised unless
    slider value is changed and handleSelectionAccepted() function is called before coming here
*/
void CxuiSettingSlider::handleClose()
{
    CX_DEBUG_ENTER_FUNCTION();

    setValue(mOriginalValue * mSliderScaleValue);
    // value selected from slider not accepted, revert back
    // the value to original value.
    CX_DEBUG(("id: %s", mSettingId.toAscii().data()));
    CX_DEBUG(("changing value back to original value: %f ", mOriginalValue));

    // when the slider is created, handlevaluechanged function is
    // called before the mSettingId is set
    if (mSettings && !mSettingId.isEmpty()){
        mSettings->set(mSettingId, mOriginalValue);
    }

    // clear settings id so that setting value doesn't get updated by accident
    // when updating slider's ui appearance
    mSettingId.clear();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Accepts the current selection.
*/
void CxuiSettingSlider::handleSelectionAccepted()
{
    CX_DEBUG_ENTER_FUNCTION();

    // sets the curren slider value to be original value. mOriginal value
    // is given to engine whenever handleClose() function is called
    if (mSliderScaleValue != 0) {
        mOriginalValue = value() / mSliderScaleValue;
    }

    emit selectionCommitted();

    CX_DEBUG_EXIT_FUNCTION();
}
