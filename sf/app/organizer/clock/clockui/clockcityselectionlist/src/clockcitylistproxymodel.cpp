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
* Definition file for class ClockCityListProxyModel.
*
*/

// System includes

// User includes
#include "clockcitylistproxymodel.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockcitylistproxymodelTraces.h"
#endif


/*!
	\class ClockCityListProxyModel

	This class acts as a filter proxy model for the city list model which we
	will use for filtering/searching in the list.

	\sa QSortFilterProxyModel
 */

/*!
	Default constructor.

	\param parent of type QObject.
 */
ClockCityListProxyModel::ClockCityListProxyModel(QObject *parent)
:QSortFilterProxyModel(parent)
{
	OstTraceFunctionEntry0( CLOCKCITYLISTPROXYMODEL_CLOCKCITYLISTPROXYMODEL_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( CLOCKCITYLISTPROXYMODEL_CLOCKCITYLISTPROXYMODEL_EXIT );
}

/*!
	From QSortFilterProxyModel.

	Called for each index in the source model. Returns true if the index
	satisfies the filter criteria.

	\sa QSortFilterProxyModel.
 */
bool ClockCityListProxyModel::filterAcceptsRow(
		int sourceRow, const QModelIndex &sourceParent) const
{
	OstTraceFunctionEntry0( CLOCKCITYLISTPROXYMODEL_FILTERACCEPTSROW_ENTRY );
	// Get the model index of the source model.
	QModelIndex modelIndex = sourceModel()->index(
			sourceRow, filterKeyColumn(), sourceParent);

	if (!modelIndex.isValid()) {
		OstTraceFunctionExit0( CLOCKCITYLISTPROXYMODEL_FILTERACCEPTSROW_EXIT );
		return false;
	}

	// Get the data corresponding to that model.
	int role = filterRole();
	QString filterExp(filterRegExp().pattern());

	QVariant modelData = sourceModel()->data(modelIndex, role);

	// We just have one item and search is done only on data set with
	// Qt::DisplayRole.
	if (Qt::UserRole + 100 == role) {
		QString string = modelData.value<QString>();
		if (string.contains(filterExp)) {
			OstTraceFunctionExit0( DUP1_CLOCKCITYLISTPROXYMODEL_FILTERACCEPTSROW_EXIT );
			return true;
		}
	}
	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

// End of file	--Don't remove this.
