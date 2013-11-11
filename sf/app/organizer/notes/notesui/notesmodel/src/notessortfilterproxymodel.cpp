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
* Definition file for class NotesSortFilterProxyModel.
*
*/

// System includes
#include <QAbstractItemModel>

// User includes
#include "notessortfilterproxymodel.h"
#include "notescommon.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "notessortfilterproxymodelTraces.h"
#endif


/*!
	\class NotesSortFilterProxyModel

	This class sub-classes the QSortFilterProxyModel to provide custom
	filtering and sorting functionality.

	\sa QSortFilterProxyModel.
 */

/*!
	Constructor.

	\param agendaUtil Reference to AgendaUtil.
	\param parent The parent.
 */
NotesSortFilterProxyModel::NotesSortFilterProxyModel(
		AgendaUtil &agendaUtil, QObject *parent)
:QSortFilterProxyModel(parent),
 mAgendaUtil(agendaUtil)
{
	OstTraceFunctionEntry0( NOTESSORTFILTERPROXYMODEL_NOTESSORTFILTERPROXYMODEL_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( NOTESSORTFILTERPROXYMODEL_NOTESSORTFILTERPROXYMODEL_EXIT );
}

/*!
	Destructor.
 */
NotesSortFilterProxyModel::~NotesSortFilterProxyModel()
{
	OstTraceFunctionEntry0( DUP1_NOTESSORTFILTERPROXYMODEL_NOTESSORTFILTERPROXYMODEL_ENTRY );
	// Nothing yet.
	OstTraceFunctionExit0( DUP1_NOTESSORTFILTERPROXYMODEL_NOTESSORTFILTERPROXYMODEL_EXIT );
}

/*!
	Re-implemented from QSortFilterProxyModel.

	\sa QSortFilterProxyModel.
 */
bool NotesSortFilterProxyModel::lessThan(
		const QModelIndex &left, const QModelIndex &right) const
{
	OstTraceFunctionEntry0( NOTESSORTFILTERPROXYMODEL_LESSTHAN_ENTRY );
	return QSortFilterProxyModel::lessThan(left, right);
}

/*!
	Re-implemented from QSortFilterProxyModel.

	\sa QSortFilterProxyModel.
 */
bool NotesSortFilterProxyModel::filterAcceptsRow(
		int sourceRow,
		const QModelIndex &sourceParent) const
{
	OstTraceFunctionEntry0( NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_ENTRY );
	// Get the model index of the source model.
	QModelIndex modelIndex = sourceModel()->index(
			sourceRow, filterKeyColumn(), sourceParent);

	// Get the data corresponding to that model.
	QVariant modelData = sourceModel()->data(modelIndex, filterRole());

	// Code for handling notes specific filter roles.
	switch (filterRole()) {
		case NotesNamespace::TypeRole:
		{
			AgendaEntry::Type type = (AgendaEntry::Type) (
					sourceModel()->data(
							modelIndex, filterRole()).value<int>());
			if (AgendaEntry::TypeNote == type
					&& QString("note").contains(filterRegExp())) {
				OstTraceFunctionExit0( NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
				return true;
			} else if (AgendaEntry::TypeTodo == type
					&& QString("todo").contains(filterRegExp())) {
				OstTraceFunctionExit0( DUP1_NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
				return true;
			}
			OstTraceFunctionExit0( DUP2_NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
			return false;
		}

		case NotesNamespace::FavouriteRole:
		{
			int favouriteStatus = sourceModel()->data(
					modelIndex, filterRole()).value<int>();

			if (1 == favouriteStatus
					&& QString("favourites").contains(filterRegExp())) {
				OstTraceFunctionExit0( DUP3_NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
				return true;
			}
			OstTraceFunctionExit0( DUP4_NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
			return false;
		}

		default:
		{
			// Now check if the role has lists, if yes, then our login goes here.
			if (QVariant::List == modelData.type() ||
					QVariant::StringList == modelData.type()) {
				QList<QVariant> dataList = modelData.toList();

				int dataCount = dataList.count();
				for (int i = 0; i < dataCount; i++) {
					QString string = dataList.value(i).toString();

					if (string.contains(filterRegExp())) {
						OstTraceFunctionExit0( DUP5_NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
						return true;
					}
				}

				OstTraceFunctionExit0( DUP6_NOTESSORTFILTERPROXYMODEL_FILTERACCEPTSROW_EXIT );
				return false;
			}
		}
	}
	// Allow the base class to check if row accepts filter.
	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

// End of file	--Don't remove this.
