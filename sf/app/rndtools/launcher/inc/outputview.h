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

#ifndef OUTPUTVIEW_H
#define OUTPUTVIEW_H

#include <hbview.h>

class HbDocumentLoader;
class HbAction;
class HbTextEdit;
class CommonActions;

class OutputView : public HbView
{
    Q_OBJECT
    
public:
    /**
     * Constructor
     */
    OutputView();
    
    /**
     * Destructor
     */
    ~OutputView();
    
    /**
     * Creates output view and adds it into main window
     */
    static OutputView* create(HbDocumentLoader &loader, CommonActions *commonActions);
    
    /**
     * Prints text into output
     */
    void printText(const QString &text);
    
private slots:

    /**
     * Clears output
     */
    void clearOutput();
    
    /**
     * Opens application view
     */
    void openApplicationView();
    
private:
    
    /**
     * Initializes view
     */
    void init(HbDocumentLoader &loader, CommonActions *commonActions);
    
    /**
     * Loads all output view's ui components from xml-file
     */
    void loadItemsFromXml(HbDocumentLoader &loader);
    
    /**
     * connects signals and slots to each other
     */
    void connectSignalsAndSlots();
    

private:
    
    // Actions
    HbAction *mActionClear;
    HbAction *mActionOpenApplicationView;  

    HbTextEdit *mTextOutput;
    
};

#endif // OUTPUTVIEW

