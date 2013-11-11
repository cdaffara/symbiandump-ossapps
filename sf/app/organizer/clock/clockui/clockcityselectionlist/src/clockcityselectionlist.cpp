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
* Description:
* Definition file for class ClockCitySelectionList.
*
*/

//System Include
#include<HbTranslator>


// User includes
#include "clockcityselectionlist.h"
#include "clockcityselectionlist_p.h"
#include "clockdatatypes.h"
#include "timezoneclient.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockcityselectionlistTraces.h"
#endif


/*!
	\class ClockCitySelectionList

	TODO: Write a complete description of the object. Mention about API usage
	as well.
 */

/*!
	\fn citySelected(LocationInfo *info)

	This signal is emitted before the cityselectionlist is closed.

	\param info Pointer to an object of LocationInfo containing the selected
	city info. If the info is 0, it means that nothing was selected.
 */

/*!
	Constructor.

	\param client Pointer to TimezoneClinet.
	\param parent of type QObject.
 */
ClockCitySelectionList::ClockCitySelectionList(
		TimezoneClient *client, QObject *parent)
:QObject(parent),
 d_ptr(new ClockCitySelectionListPrivate(client, this))
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLIST_CLOCKCITYSELECTIONLIST_ENTRY );
	// Load the translation file and install the editor specific translator
	mTranslator = new HbTranslator("clockcityselectionlist");
	mTranslator->loadCommon();
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLIST_CLOCKCITYSELECTIONLIST_EXIT );
}

/*!
	Default constructor.

	\param parent of type QObject.
 */
ClockCitySelectionList::ClockCitySelectionList(QObject *parent)
:QObject(parent),
 d_ptr(new ClockCitySelectionListPrivate(0, this))
{
	OstTraceFunctionEntry0( DUP1_CLOCKCITYSELECTIONLIST_CLOCKCITYSELECTIONLIST_ENTRY );
	// Load the translation file and install the editor specific translator
	mTranslator = new HbTranslator("clockcityselectionlist");
	mTranslator->loadCommon();
	OstTraceFunctionExit0( DUP1_CLOCKCITYSELECTIONLIST_CLOCKCITYSELECTIONLIST_EXIT );
}

ClockCitySelectionList::~ClockCitySelectionList()
{
    OstTraceFunctionEntry0( DUP2_CLOCKCITYSELECTIONLIST_CLOCKCITYSELECTIONLIST_ENTRY );
    // Remove the translator
    if (mTranslator) {
        delete mTranslator;
         mTranslator = 0;
    }
    OstTraceFunctionExit0( DUP2_CLOCKCITYSELECTIONLIST_CLOCKCITYSELECTIONLIST_EXIT );
}

/*!
	Shows the city selection list. Connect to signal citySelected before calling this function
	to make sure that the notification is reached upon selecting a city.
 */
void ClockCitySelectionList::showCityList()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLIST_SHOWCITYLIST_ENTRY );
	d_ptr->showCityList();
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLIST_SHOWCITYLIST_EXIT );
}

/*!
	Closes the city list selection list. This is a convenient API for the client to close the list
	if required. This would emit the citySelected signal without any selected city information.
 */
void ClockCitySelectionList::closeCityList()
{
	OstTraceFunctionEntry0( CLOCKCITYSELECTIONLIST_CLOSECITYLIST_ENTRY );
	d_ptr->closeCityList();
	OstTraceFunctionExit0( CLOCKCITYSELECTIONLIST_CLOSECITYLIST_EXIT );
}

// End of file	--Don't remove this.
