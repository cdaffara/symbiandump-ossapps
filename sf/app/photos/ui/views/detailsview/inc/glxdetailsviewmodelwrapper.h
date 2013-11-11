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
 * Description:   ?Description
 *
 */

#ifndef GLXDETAILSVIEWMODELWRAPPER
#define GLXDETAILSVIEWMODELWRAPPER

//Includes
#include <QAbstractItemModel>

/**
 * Class Description
 * This is Wrapper class,which access the model [GlxMediaModel] for the widgets created in the details view
 * Here it specifies the no:of custom widgets to be created in the list view for the details view
 */

class GlxDetailsViewModelWrapper : public QAbstractItemModel
    {
Q_OBJECT
public:

    /**
     * Constructor
     * @param - QAbstractItemModel object, this object is passed from the details view, while creating this
     *          class object 
     */
    GlxDetailsViewModelWrapper(QAbstractItemModel *model);

    /**
     * Destructor
     */
    ~GlxDetailsViewModelWrapper();

    /**
     * From QAbstractItemModel
     * This Function specifies the no:of custom widgets to be created as well
     * the no:of custom widgets present.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * From QAbstractItemModel
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * From QAbstractItemModel
     */
    QModelIndex parent(const QModelIndex &child) const;

    /**
     * From QAbstractItemModel
     * This Function returns the data fetched from the Model corresponding to the role passed
     */
    QVariant data(const QModelIndex &index, int role) const;

    /**
     * From QAbstractItemModel
     * This Function verifies if the index passed is valid or not.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent =
            QModelIndex()) const;

    /**
     * This Function Emits the Signal for the Custom widgets to refresh  thier data shown
     */
    void refreshCustomWidget();

private:
    
    /**
     * This Function returns the size in standard form like Bytes,KB,MB,GB
     * @param :size , specified in Bytes 
     */
    QString sizeinStrings(int size) const;

signals:
    /*
     * This Signal Specifies Custom widgets to refresh  thier data shown as there is change in data in the model 
     */
    void updateCustomWidgets();
private:

    //This variable is initialised with model passed by the view, while creating this class object
    QAbstractItemModel *mModel;
    };

#endif //GLXDETAILSVIEWMODELWRAPPER_H
