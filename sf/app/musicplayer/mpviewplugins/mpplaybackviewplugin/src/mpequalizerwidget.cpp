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
* Description: Equalizer widget for Music Player playback view.
*
*/

// System includes
#include <QStringList>
#include <hblabel.h>
#include <hbaction.h>
#include <hbradiobuttonlist.h>
#include <EqualizerConstants.h>

// User includes
#include "mpequalizerwidget.h"
#include "mpenginefactory.h"
#include "mptrace.h"


/*!
    \class MpEqualizerWidget
    \brief Provide radio button list to select a predefined eq. preset
*/

/*!
 Constructs the Equalizer widget.
 */
MpEqualizerWidget::MpEqualizerWidget() :
    mMpEngine(0),
    mPresetsList(0),
    mOriginalPresetIndex(0)
{
    TX_ENTRY
    
    initialize();
    
    TX_EXIT
}

/*!
 Destructs the widget.
 */
MpEqualizerWidget::~MpEqualizerWidget()
{
	TX_LOG
}

/*!
 Initialize dialog by loading the EqualizerUtility.
 */
void MpEqualizerWidget::initialize()
{
    TX_ENTRY
    
    mMpEngine = MpEngineFactory::sharedEngine();

    TX_EXIT
}

/*!
 Prepare dialog by loading the presets list.
 */
void MpEqualizerWidget::prepareDialog()
{
    TX_ENTRY
    
    HbAction *action;
    setTimeout(NoTimeout);
	setModal( true );
    setDismissPolicy(HbDialog::NoDismiss);
    setFrameType(HbPopup::Strong);
    setHeadingWidget(new HbLabel(hbTrId("txt_mus_title_select_preset")));

    mOriginalPresetIndex = mMpEngine->activePreset();
    QStringList listItems;
    QStringList presetNames = mMpEngine->presetNames();
    for ( int i=0; i<presetNames.count(); i++ ) {
        QString name = getLocalizedString( presetNames.at(i) );
        if ( name.size() > 0 ) {
            listItems.append( name );
        }
        else {
            // Localized string not found, append original name.
            TX_LOG_ARGS( "Localized string not found = " << name);
            listItems.append( presetNames.at(i) );
        }
    }
    listItems.prepend(hbTrId("txt_mus_list_off"));
    TX_LOG_ARGS( "PresetNames=" << listItems);
    
    mPresetsList = new HbRadioButtonList(listItems, mOriginalPresetIndex, HbRadioButtonList::NoPreview, this);
    setContentWidget( mPresetsList );  //mPresetsList now owned by HbDialog

    connect( mPresetsList, 
           SIGNAL(itemSelected(int)), 
           this, 
           SLOT(presetSelected(int)));
    
    clearActions();
    action = new HbAction( hbTrId( "txt_common_button_ok" ) );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( okSelected( bool ) ) );
    addAction( action );
    action = new HbAction( hbTrId( "txt_common_button_cancel" ) );
    connect( action, SIGNAL( triggered( bool ) ), this, SLOT( cancelSelected( bool ) ) );
    addAction( action );
    
    TX_EXIT
}

/*!
 Slot to handle when a preset has been selected.
 */
void MpEqualizerWidget::presetSelected(int index)
{
    TX_ENTRY
    
    if (mMpEngine) {
        if (index == 0) {
            mMpEngine->disableEqualizer();
        }
        else{
            mMpEngine->applyPreset( index );
        }
    }
    
    TX_EXIT
}

/*!
 Slot to handle when Cancel is selected
 */
void MpEqualizerWidget::cancelSelected(bool checked)
{
    TX_ENTRY
    Q_UNUSED(checked);
    
    if (mMpEngine && mPresetsList) {
        // Return to the original Preset
        if (mOriginalPresetIndex != mMpEngine->activePreset()) {
            if (mOriginalPresetIndex == 0) {
                mMpEngine->disableEqualizer();
                mPresetsList->setSelected( 0 );     //First list item corresponds to "OFF"
            }
            else {
                mMpEngine->applyPreset( mOriginalPresetIndex );
                mPresetsList->setSelected( mOriginalPresetIndex );
            }            
        }
    }
    
    TX_EXIT
}

/*!
 Slot to handle when Ok is selected
 */
void MpEqualizerWidget::okSelected(bool checked)
{
    TX_ENTRY
    Q_UNUSED(checked);    
    
    //Update original preset
    if (mMpEngine) {
        mOriginalPresetIndex = mMpEngine->activePreset();
    }
    
    TX_EXIT
}

/*!
  Return the localized string base on preset name.
 */
QString MpEqualizerWidget::getLocalizedString( QString name )
{
    TX_ENTRY_ARGS( "Name = " << name );
    QString localizedString;

    if ( name == "Bass Booster" ) {
        localizedString = hbTrId( "txt_mus_list_bass_booster" );
    }
    else if ( name == "Classic" ) {
        localizedString = hbTrId( "txt_mus_list_classical" );
    }
    else if ( name == "Pop" ) {
        localizedString = hbTrId( "txt_mus_list_pop" );
    }
    else if ( name == "Jazz" ) {
        localizedString = hbTrId( "txt_mus_list_jazz" );
    }
    else if ( name == "Rock" ) {
        localizedString = hbTrId( "txt_mus_list_rock" );
    }
    
    TX_EXIT_ARGS( "Logical String = " << localizedString );
    return localizedString;
}

//End of File
