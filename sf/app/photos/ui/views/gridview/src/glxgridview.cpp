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



//Includes
#include <hbmainwindow.h>
#include <hggrid.h>
#include <glxmodelwrapper.h>
#include <hbpushbutton.h>
#include <HbToolBar> 
#include <HbAction>
#include <hbiconitem.h>
#include <hbicon.h>
#include <xqserviceutil.h>
#include <hbcheckbox.h>
#include <hblabel.h>
#include <QString>
#include <hbframeitem.h>
#include <hbgroupbox.h>
#include <hbparameterlengthlimiter.h>
#include <QGraphicsLinearLayout>

//User Includes
#include "glxviewids.h"
#include "glxgridview.h"
#include "glxmodelparm.h"
#include "glxcommandhandlers.hrh"
#include "glxicondefs.h"
#include "glxlocalisationstrings.h"
#include "glxsettinginterface.h"


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "glxgridviewTraces.h"
#endif

GlxGridView::GlxGridView(HbMainWindow *window)
    : GlxView ( GLX_GRIDVIEW_ID ),
      mWindow(window),
      mModel ( NULL),
      mWidget(NULL),
      mSelectionModel(NULL),
      mModelWrapper(NULL),
      mUiOnButton(NULL),      
      mScrolling(FALSE),
      mIconItem(NULL),
      mMarkCheckBox(NULL),
      mTotalImagesCount(NULL),
      mMarkSelectHeading(NULL),
      mMarkCountLabel(NULL),
      mZeroItemLabel(NULL),
      mAlbumNameHeading(NULL),
      mMarkContainer(NULL),
      mMarkingWidget(NULL),
      mToolBar( NULL ),
      mCurrentToolBar( NULL )
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_GLXGRIDVIEW_ENTRY );
    mModelWrapper = new GlxModelWrapper();
    mModelWrapper->setRoles(GlxQImageSmall);
    mIconItem = new HbIconItem(this);
    mSettings = GlxSettingInterface::instance() ;
    
    mUiOnButton = new HbPushButton(this);
    connect(mUiOnButton, SIGNAL(clicked(bool)), this, SLOT(uiButtonClicked(bool)));
    mUiOnButton->setGeometry(QRectF(590,0,40,40));
    mUiOnButton->setZValue(1);
    mUiOnButton->setIcon(HbIcon(GLXICON_WALL_UI_ON));
    mUiOnButton->setObjectName( "UiOn Button" );

    OstTraceFunctionExit0( GLXGRIDVIEW_GLXGRIDVIEW_EXIT );
}

void GlxGridView::activate()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_ACTIVATE_ENTRY );
    loadGridView();
	connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationchanged(Qt::Orientation)),Qt::UniqueConnection);
    if(mTotalImagesCount == NULL) {
        mTotalImagesCount = new HbGroupBox(this);
        mTotalImagesCount->setObjectName( "Count" );
        mTotalImagesCount->hide();
    }
    mWidget->resetTransform();
    OstTraceFunctionExit0( GLXGRIDVIEW_ACTIVATE_EXIT );
}

void GlxGridView::deActivate()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_DEACTIVATE_ENTRY );
    mScrolling = FALSE;
    if (mUiOnButton){
        mUiOnButton->hide();
    }
    if(mIconItem) {
        mIconItem->hide();
        mIconItem->resetTransform();
        mIconItem->setOpacity(0);
        mIconItem->setZValue(mIconItem->zValue()-20);
    }
    if (mTotalImagesCount) {
        mTotalImagesCount->hide();
    }
    if (mAlbumNameHeading) {
        mAlbumNameHeading->hide();
    }
    if(mZeroItemLabel) {
        mZeroItemLabel->hide();
    }
    
    disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(orientationchanged(Qt::Orientation)));
    OstTraceFunctionExit0( GLXGRIDVIEW_DEACTIVATE_EXIT );
}

void GlxGridView::initializeView( QAbstractItemModel *model, GlxView *preView )
{
    Q_UNUSED( preView )
    activate();
    setModel(model);
}

void GlxGridView::clearCurrentModel()
{
    if ( mModel ) {
        disconnect( mModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( rowsInserted() ) );
        disconnect( mModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( rowsRemoved() ) );
        disconnect( mModel, SIGNAL(destroyed()), this, SLOT( clearCurrentModel() ) );
        disconnect( mModel, SIGNAL( albumTitleAvailable( QString ) ), this, SLOT( showAlbumTitle( QString ) ) );
        disconnect( mModel, SIGNAL( populated() ), this, SLOT( populated() ) );
        mModel = NULL;
    }
}

void GlxGridView::initializeNewModel()
{
    if ( mModel ) {
        connect( mModel, SIGNAL( rowsInserted( QModelIndex, int, int ) ), this, SLOT( rowsInserted() ) );
        connect( mModel, SIGNAL( rowsRemoved( QModelIndex, int, int ) ), this, SLOT( rowsRemoved() ) );
        connect( mModel, SIGNAL(destroyed()), this, SLOT( clearCurrentModel() ) );
        if ( getSubState() == ALBUM_ITEM_S ) {
            connect( mModel, SIGNAL( albumTitleAvailable( QString ) ), this, SLOT( showAlbumTitle( QString ) ) );
        }
        connect( mModel, SIGNAL( populated() ), this, SLOT( populated() ) );
    }
}

void GlxGridView::setModel( QAbstractItemModel *model )
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_SETMODEL_ENTRY );
    if( model ) {
        clearCurrentModel();
        mModel = model;
        initializeNewModel();
        
        QVariant variantimage = mModel->data(mModel->index(0,0),GlxDefaultImage);
        if ( variantimage.isValid() && variantimage.canConvert<QImage> () ) {
            mWidget->setDefaultImage( variantimage.value<QImage>() );
        }
        
        mModelWrapper->setModel( mModel );
        mWidget->setModel( mModelWrapper );
        
        if( !mSelectionModel ) {
            mSelectionModel = new QItemSelectionModel( mModelWrapper, this );
            connect( mSelectionModel, SIGNAL( currentChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT( visibleIndexChanged( const QModelIndex &, const QModelIndex & ) ) );
            mWidget->setSelectionModel( mSelectionModel );
        }
        
        scrolltofocus(); // Need to do it here ?
        showItemCount();
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_SETMODEL_EXIT );
}

void GlxGridView::visibleIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if(mScrolling)
        {
        Q_UNUSED(previous);
        if(current.row() >= 0 || current.row() < mModel->rowCount())
            {
            mModel->setData(mModel->index(0,0),current.row(),GlxVisualWindowIndex);
            }
        }
}

void GlxGridView::addToolBar( HbToolBar *toolBar )
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_ADDTOOLBAR_ENTRY );
    
    //For album grid view, add and remove from album tool bar will be shown
    int subState = getSubState();
    if ( subState == ALBUM_ITEM_S && mWidget->selectionMode() == HgWidget::NoSelection ) {
        if ( !mToolBar ) {
            createAlbumGridToolBar();
        }
        if ( mToolBar ) {
            //remove from action will be disable if number of images in the album grid is zero.
            if ( mModel->rowCount()  ) {
                mToolBar->actions().at(0)->setEnabled( true ); //To:Do remove once selection dialog is implemented
                mToolBar->actions().at(1)->setEnabled( true );
            }
            else {
                mToolBar->actions().at(0)->setEnabled( false );
                mToolBar->actions().at(1)->setEnabled( false );
            }
            if ( mToolBar != mCurrentToolBar ) {
                takeToolBar();
                setToolBar( mToolBar );
                mCurrentToolBar = mToolBar;
            }
        }
    }
    else {
        if ( toolBar != mCurrentToolBar ) {
            takeToolBar();
            setToolBar( toolBar );
            mCurrentToolBar = toolBar;
        }
    }
    
    showHbItems();
    OstTraceFunctionExit0( GLXGRIDVIEW_ADDTOOLBAR_EXIT );
}

void GlxGridView::enableMarking()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ENABLEMARKING, "GlxGridView::enableMarking" );
    mWidget->setSelectionMode(HgWidget::MultiSelection);
   
    if (mMarkingWidget == NULL) {
        mMarkingWidget = new HbWidget(this);
        mMarkContainer = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        mMarkingWidget->setLayout(mMarkContainer);

        mMarkSelectHeading = new HbGroupBox(this);
        mMarkSelectHeading->setHeading(GLX_SELECT_IMAGES);
        mMarkSelectHeading->setObjectName("Select Photos");
               
        mMarkCountLabel = new HbLabel(mMarkingWidget);
        mMarkCountLabel->setObjectName("MarkCount");
        mMarkCountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        
        mMarkCheckBox = new HbCheckBox(GLX_LABEL_MARK_ALL, mMarkingWidget);
        mMarkCheckBox->setObjectName("CheckB MarkAll");
       
        mMarkContainer->addItem(mMarkCheckBox);
        mMarkContainer->addItem(mMarkCountLabel);        
    }

    hideorshowitems( mWindow->orientation() );

    connect( mWidget->selectionModel() , SIGNAL( selectionChanged( const QItemSelection &, const QItemSelection& ) ), this, SLOT( showMarkedItemCount() ) );
    connect( mMarkCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( stateChanged( int ) ));
}

void GlxGridView::disableMarking()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_DISABLEMARKING, "GlxGridView::disableMarking" );
    mWidget->setSelectionMode(HgWidget::NoSelection);
    disconnect( mWidget->selectionModel() , SIGNAL( selectionChanged(const QItemSelection &, const QItemSelection& ) ), this, SLOT( showMarkedItemCount() ) );
    disconnect(mMarkCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( stateChanged(int)));
    
    if (mMarkSelectHeading) {
        mMarkSelectHeading->hide();
    }

    if (mMarkingWidget) {
        mMarkingWidget->hide();
    }     
    
    if (mMarkCheckBox) {
       mMarkCheckBox->setCheckState(Qt::Unchecked);
    }
	
    showHbItems();
}

void GlxGridView::stateChanged(int state)
{
    if(state)
        handleUserAction(EGlxCmdMarkAll);
    else
        handleUserAction(EGlxCmdUnMarkAll);
}

void GlxGridView::showMarkedItemCount()
{
    int count = mModel->rowCount();
    QModelIndexList indexList = mWidget->selectionModel()->selectedIndexes();
    int markItemCount = indexList.count();
    QString text = HbParameterLengthLimiter(GLX_LABEL_MARK_COUNT).arg(markItemCount).arg(count);
    mMarkCountLabel->setPlainText(text);

    //Block The signals generated by the CheckBox
    //if we dont block the signals  , then when we call setCheckState to selected and unselect the CheckBox
    //it causes state change signal to emit which will cause either mark all or un mark all of teh images.
    mMarkCheckBox->blockSignals(TRUE);
    if (markItemCount != count && mMarkCheckBox->checkState() == Qt::Checked) {
        //if the mark all option was checked  and then the user un marks an slected item
        //Then we have to un check the Mark All check Box
        mMarkCheckBox->setCheckState(Qt::Unchecked);
    }
    else if (markItemCount == count) {
        //if all the images are selected/marked Indiduvally
        //Then the the Mark All check Box has to checked/Selected
        mMarkCheckBox->setCheckState(Qt::Checked);
    }
    //UnBlock The signals generated by the CheckBox
    mMarkCheckBox->blockSignals(FALSE);
}

void GlxGridView::showItemCount()
{
    int count = 0;
    if( mModel ) {
        count = mModel->rowCount();
        QSize deviceSize = HbDeviceProfile::current().logicalSize();
        QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                       : QSize( deviceSize.height(), deviceSize.width() )  ;
        if( count ) {
            if(mZeroItemLabel) {
                mZeroItemLabel->hide();
            }
            
            if( isItemVisible( Hb::TitleBarItem ) ) {
                QString text;
				if(XQServiceUtil::isService() && (0 == XQServiceUtil::interfaceName().compare(QLatin1String("com.nokia.symbian.IImageFetch")))) {
					showAlbumTitle(GLX_SELECT_IMAGE);
				}
                else if ( getSubState() == ALL_ITEM_S || ( getSubState() == FETCHER_ITEM_S)) {
					if (mAlbumNameHeading) {
                    	mAlbumNameHeading->hide();
					}
				
                    text = HbParameterLengthLimiter(GLX_GRID_VIEW_COUNT_LABEL, count); 
                    mTotalImagesCount->setHeading ( text );
                    mTotalImagesCount->setGeometry(QRectF(0,0,screenSize.width(),deviceSize.height()/24));
                    mTotalImagesCount->show();
                }
                else if ( getSubState() == ALBUM_ITEM_S || (getSubState() ==  FETCHER_ALBUM_ITEM_S)) {
                    mTotalImagesCount->hide();
                    QVariant variant = mModel->data(mModel->index(0,0),GlxViewTitle);
                    if (variant.toString() != NULL) {
                        showAlbumTitle(variant.toString());
                    }
                }
            }
            else {
                if (mTotalImagesCount) {
                    mTotalImagesCount->hide();
                }
                if (mAlbumNameHeading) {
                    mAlbumNameHeading->hide();
                }
            }

        }
        else {
            bool populated = FALSE;
            QVariant variant = mModel->data(mModel->index(0,0),GlxPopulated);
            if (variant.isValid() &&  variant.canConvert<bool>() )
                {
                populated = variant.value<bool>();
                }
            if(populated) {
                if (mTotalImagesCount) {
                    mTotalImagesCount->hide();
                }
                if (mAlbumNameHeading) {
                    mAlbumNameHeading->hide();
                }

                showNoImageString();

                if (getSubState() == ALBUM_ITEM_S) {
                    QVariant variant = mModel->data(mModel->index(0,0),GlxViewTitle);
                    if (variant.toString() != NULL) {
                        showAlbumTitle(variant.toString());
                    }
				}	
            }
        }
    }
}

void GlxGridView::showAlbumTitle(QString aTitle)
{
    int count = mModel->rowCount();
    QSize deviceSize = HbDeviceProfile::current().logicalSize();
    QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                   : QSize( deviceSize.height(), deviceSize.width() )  ;
    if (mAlbumNameHeading == NULL)
        {
        mAlbumNameHeading = new HbGroupBox(this);
        mAlbumNameHeading->setObjectName("Album Name");
        }

    //If fetcher service set only title text
    if ((XQServiceUtil::isService()) && isItemVisible(Hb::TitleBarItem))
        {       
        mAlbumNameHeading->setHeading(aTitle);
        mAlbumNameHeading->setGeometry(QRectF(0, 0, screenSize.width(),deviceSize.height() / 24));       
        mAlbumNameHeading->show();
        }
    else
        { //handle album tiltle and count display logic here
        if (isItemVisible(Hb::TitleBarItem))
            {
            QString text = HbParameterLengthLimiter(GLX_ALBUM_NAME_COUNT_LABEL).arg(aTitle).arg(count);    
            mAlbumNameHeading->setHeading(text);
            mAlbumNameHeading->setGeometry(QRectF(0, 0, screenSize.width(),deviceSize.height()/24));                 
            mAlbumNameHeading->show();
            }
        }
}

void GlxGridView::showNoImageString()
{
    qreal chromeHeight = 0.0;
    QSize deviceSize = HbDeviceProfile::current().logicalSize();
    QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                   : QSize( deviceSize.height(), deviceSize.width() )  ;
    qreal midHeight = screenSize.height()/2;
    if (isItemVisible(Hb::TitleBarItem)) {
        style()->parameter("hb-param-widget-chrome-height", chromeHeight);
        midHeight -= chromeHeight;
    }
    if (mZeroItemLabel == NULL) {
        QString displayText(GLX_GRID_NO_IMAGE); 
        mZeroItemLabel = new HbLabel(displayText, this);
        mZeroItemLabel->setObjectName( "No Image" );
    }
    mZeroItemLabel->setGeometry(QRectF(0, midHeight - deviceSize.height()/16, screenSize.width(), 3*deviceSize.height()/32));
    mZeroItemLabel->setAlignment(Qt::AlignHCenter);
    mZeroItemLabel->show();
   
}

void GlxGridView::populated()
{
    QVariant variant = mModelWrapper->data(mModelWrapper->index(0,0), GlxVisualWindowIndex );
    int visualIndex = 0;
    if ( variant.isValid() &&  variant.canConvert<int> () )  {
        visualIndex = variant.value<int>();
    }
    mWidget->scrollTo(mModelWrapper->index(visualIndex,0));
    showItemCount();
}

void GlxGridView::rowsInserted()
{
    showItemCount();
    if( getSubState() == ALBUM_ITEM_S  && mToolBar ) {
        mToolBar->actions().at(0)->setEnabled( true ); //To:Do remove once selection dialog is implemented
        mToolBar->actions().at(1)->setEnabled( true );
    }
}

void GlxGridView::rowsRemoved()
{
    showItemCount(); 
    if( getSubState() == ALBUM_ITEM_S  && mModel->rowCount() == 0 && mToolBar ) {
        mToolBar->actions().at(0)->setEnabled( false ); //To:Do remove once selection dialog is implemented
        mToolBar->actions().at(1)->setEnabled( false );
    }    
}

void GlxGridView::handleToolBarAction()
{
    HbAction *action = qobject_cast<HbAction*>( sender() );
    qint32 commandId = action->data().toInt();
    emit actionTriggered( commandId );
}

void GlxGridView::handleUserAction(qint32 commandId)
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_HANDLEUSERACTION, "GlxGridView::handleUserAction" );
    switch( commandId ) {
        case EGlxCmdMarkAll :
            mWidget->selectAll();
            break;

        case EGlxCmdUnMarkAll :
            mWidget->clearSelection();
            break;

        case EGlxCmd3DEffectOn:
            mSettings->setmediaWall3DEffect(1);
            if(mWidget && !mWidget->effect3dEnabled())
                mWidget->setEffect3dEnabled(ETrue);
            break;
            
        case EGlxCmd3DEffectOff:
            mSettings->setmediaWall3DEffect(0);
            if(mWidget && mWidget->effect3dEnabled())
                mWidget->setEffect3dEnabled(EFalse);
            break;

        default :
            break;
    }
}

QItemSelectionModel * GlxGridView::getSelectionModel()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_GETSELECTIONMODEL, "GlxGridView::getSelectionModel" );
    return mWidget->selectionModel();
}

QGraphicsItem * GlxGridView::getAnimationItem(GlxEffect transitionEffect)
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_GETANIMATIONITEM_ENTRY );
    int selIndex = -1;

    if ( transitionEffect == FULLSCREEN_TO_GRID  || transitionEffect == FULLSCREEN_TO_GRID_PORTRAIT ) {
        return mWidget;
    }

    if ( transitionEffect == GRID_TO_FULLSCREEN ) {
        QVariant variant = mModel->data( mModel->index( 0, 0 ), GlxFocusIndexRole );
        if ( variant.isValid() && variant.canConvert<int> () ) {
            selIndex = variant.value<int> ();
        }
        else {
            return mWidget;
        }
        QVariant variantimage = mModel->data( mModel->index( selIndex, 0 ), Qt::DecorationRole );
        if ( variantimage.isValid() && variantimage.canConvert<HbIcon> () ) {
            QPolygonF poly;
            if ( mModelWrapper && !mWidget->getItemOutline( mModelWrapper->index( selIndex, 0 ), poly ) ) {
                return mWidget;
            }
            QRectF itemRect = poly.boundingRect();

            mIconItem->setSize( QSize( 120, 120 ) );
            HbIcon tempIcon = variantimage.value<HbIcon> ();
            QPixmap tempPixmap = tempIcon.qicon().pixmap( 120, 120 );
            QSize sz = QSize( 120, 120 );
            tempPixmap = tempPixmap.scaled( sz, Qt::IgnoreAspectRatio );
            HbIcon tmp = HbIcon( QIcon( tempPixmap ) );
            mIconItem->setIcon( tmp );
            mIconItem->setPos( itemRect.topLeft() );
            mIconItem->setZValue( mIconItem->zValue() + 20 );
            mIconItem->show();
            if ( mToolBar ) {
                mToolBar->setZValue( mIconItem->zValue() - 5 );
            }
            return mIconItem;
        }
        else {
            return mWidget;
        }
    }

    if ( transitionEffect == GRID_TO_ALBUMLIST || transitionEffect == ALBUMLIST_TO_GRID ) {
        return mWidget;
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_GETANIMATIONITEM_EXIT );
    return NULL;
}

void GlxGridView::loadGridView()
{
    OstTraceFunctionEntry0( GLXGRIDVIEW_LOADGRIDVIEW_ENTRY );
    if(mWidget == NULL) {
        Qt::Orientation orient = mWindow->orientation();
        mWindow->viewport()->grabGesture(Qt::PanGesture);
        mWindow->viewport()->grabGesture(Qt::TapGesture);
        mWindow->viewport()->grabGesture(Qt::TapAndHoldGesture);
        mWidget = new HgGrid(orient);
        mWidget->setObjectName( "Media Wall" );
        mWidget->setLongPressEnabled(true);
        mWidget->setScrollBarPolicy(HgWidget::ScrollBarAutoHide);
        if(XQServiceUtil::isService())
            {
            mWidget->setEffect3dEnabled(EFalse);
            }
        else
            {
            mWidget->setEffect3dEnabled(mSettings->mediaWall3DEffect());
            }
        setWidget( mWidget );
        addViewConnection();
    }
    OstTraceFunctionExit0( GLXGRIDVIEW_LOADGRIDVIEW_EXIT );
}

void GlxGridView::orientationchanged(Qt::Orientation orient)
{
    if (mWidget && mWidget->selectionMode() == HgWidget::MultiSelection) {
        hideorshowitems(orient);
    }
    showHbItems();
}

void GlxGridView::hideorshowitems(Qt::Orientation orient)
{
    if ( mWidget && mWidget->selectionMode() == HgWidget::NoSelection ) {
        if( orient == Qt::Horizontal ) {
			setItemVisible(Hb::AllItems, FALSE) ;
        	setViewFlags(viewFlags() | HbView::ViewTitleBarHidden | HbView::ViewStatusBarHidden);
            showItemCount();
            if(mUiOnButton == NULL) {
                mUiOnButton = new HbPushButton(this);
                connect(mUiOnButton, SIGNAL(clicked(bool)), this, SLOT(uiButtonClicked(bool)));
                mUiOnButton->setGeometry(QRectF(590,0,40,40));
                mUiOnButton->setZValue(1);
                mUiOnButton->setIcon(HbIcon(GLXICON_WALL_UI_ON));
                mUiOnButton->setObjectName( "UiOn Button" );
            }
            mUiOnButton->show();
        }
        else {
            showHbItems();
        }
    }
    else if (mWidget && mWidget->selectionMode() == HgWidget::MultiSelection) {
        setItemVisible(Hb::AllItems, FALSE) ;
        setViewFlags(viewFlags() | HbView::ViewTitleBarHidden | HbView::ViewStatusBarHidden);
        if (mUiOnButton) {
            mUiOnButton->hide();
        }
        if (mTotalImagesCount) {
            mTotalImagesCount->hide();
        }
        if (mAlbumNameHeading) {
            mAlbumNameHeading->hide();
        }
        
        QSize deviceSize = HbDeviceProfile::current().logicalSize();
        QSize screenSize = ( mWindow->orientation() == Qt::Vertical ) ? QSize( deviceSize.width(), deviceSize.height() )
                                                                       : QSize( deviceSize.height(), deviceSize.width() )  ;
        
        mMarkSelectHeading->setGeometry(QRectF(0,0,screenSize.width(),deviceSize.height()/24));       
        mMarkingWidget->setGeometry(QRectF(0,deviceSize.height()/24,screenSize.width(),deviceSize.height()/72));          
              
        mMarkSelectHeading->show();
        mMarkingWidget->show();        
        showMarkedItemCount();        
    }
}

void GlxGridView::scrolltofocus()
{
    if(mModelWrapper && mWidget)
        {
        QVariant variant = mModelWrapper->data( mModelWrapper->index(0,0), GlxFocusIndexRole );
        if ( variant.isValid() &&  variant.canConvert<int> () )
            {
            mWidget->scrollTo( mModelWrapper->index( variant.value<int>(),0) );
            mModel->setData(mModel->index(0,0),variant.value<int>(),GlxVisualWindowIndex);
            }
        }
}


void GlxGridView::addViewConnection()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_ADDVIEWCONNECTION, "GlxGridView::addViewConnection" );
    //connect(mWindow, SIGNAL(aboutToChangeOrientation()), mWidget, SLOT(aboutToChangeOrientation()));
    connect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), mWidget, SLOT(orientationChanged(Qt::Orientation)));
    connect(mWidget, SIGNAL(activated(const QModelIndex &)), SLOT( itemSelected(const QModelIndex &)));
    connect(mWidget, SIGNAL( scrollingStarted() ), this, SLOT( scrollingStarted() ) );
    connect(mWidget, SIGNAL( scrollingEnded() ), this, SLOT( scrollingEnded() ) );
    connect(mWidget, SIGNAL(longPressed(const QModelIndex &, QPointF)), SLOT( indicateLongPress(const QModelIndex &, QPointF) ) );
}

void GlxGridView::removeViewConnection ()
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_REMOVEVIEWCONNECTION, "GlxGridView::removeViewConnection" );
    if(mWidget)
        {
        //disconnect(mWindow, SIGNAL(aboutToChangeOrientation()), mWidget, SLOT(aboutToChangeOrientation()));
        disconnect(mWindow, SIGNAL(orientationChanged(Qt::Orientation)), mWidget, SLOT(orientationChanged(Qt::Orientation)));
        disconnect(mWidget, SIGNAL(activated(const QModelIndex &)),this, SLOT( itemSelected(const QModelIndex &)));
        disconnect(mWidget, SIGNAL( scrollingStarted() ), this, SLOT( scrollingStarted() ) );
        disconnect(mWidget, SIGNAL( scrollingEnded() ), this, SLOT( scrollingEnded() ) );
        disconnect(mWidget,  SIGNAL(longPressed(const QModelIndex &, QPointF)),this, SLOT( indicateLongPress(const QModelIndex &, QPointF) ) );
        }
}


void GlxGridView::itemSelected(const QModelIndex &  index)
{
    OstTrace1( TRACE_NORMAL, GLXGRIDVIEW_ITEMSELECTED, "GlxGridView::itemSelected;index=%d", index.row() );
    if ( mWidget->selectionMode() == HgWidget::MultiSelection ) {
        return ;
    }
    OstTraceEventStart0( EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START, "Fullscreen Launch Time" );
    if ( mModel ) {
        mModel->setData( index, index.row(), GlxFocusIndexRole );
    }
    if(XQServiceUtil::isService() ){
		if(0 == XQServiceUtil::interfaceName().compare(QLatin1String("com.nokia.symbian.IImageFetch"))){
	        qDebug("GlxGridView::itemSelected actionTriggered( EGlxCmdFetcherSelect )" );
	        emit actionTriggered( EGlxCmdFetcherSelect );
		}
		else{
			emit actionTriggered( EGlxCmdBrowseFullScreenOpen);
		}
    }
	else{
		emit actionTriggered( EGlxCmdFullScreenOpen );
	}
    OstTraceEventStop( EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_STOP, "Fullscreen Launch Time", EVENT_DUP1_GLXGRIDVIEW_ITEMSELECTED_START );
}

void GlxGridView::scrollingStarted()
{
    if ((mWindow->orientation() == Qt::Horizontal)/* && mWidget->selectionMode() == HgWidget::NoSelection*/)
        {
        setItemVisible(Hb::AllItems, FALSE) ;
        setViewFlags(viewFlags() | HbView::ViewTitleBarHidden | HbView::ViewStatusBarHidden);
        if (mUiOnButton)
            {
            mUiOnButton->hide();
            }
		if (mTotalImagesCount) {
			mTotalImagesCount->hide();
		}
		if (mAlbumNameHeading) {
            mAlbumNameHeading->hide();
		}
    }

    mScrolling = TRUE;
}

void GlxGridView::scrollingEnded()
{
    mScrolling = FALSE;
    if ( mUiOnButton && ( mWindow->orientation() == Qt::Horizontal ) ) {
        mUiOnButton->show();
    }
    
    QList<QModelIndex> visibleIndex = mWidget->getVisibleItemIndices();
    if ( visibleIndex.count() <= 0 ) {
        return;
    }
    QModelIndex index = visibleIndex.at(0);

    if(mModel) {
        if ( index.row() < 0 || index.row() >= mModel->rowCount() ) {
            return;
        }
        mModel->setData( index, index.row(), GlxVisualWindowIndex );
    }
}

GlxGridView::~GlxGridView()
{
    OstTraceFunctionEntry0( DUP1_GLXGRIDVIEW_GLXGRIDVIEW_ENTRY );
    removeViewConnection();
    if(mSelectionModel)
        {
        disconnect(mSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(visibleIndexChanged(const QModelIndex &, const QModelIndex &)));
        delete mSelectionModel;
        }
    delete mWidget;
    delete mModelWrapper;
    if(mUiOnButton) {
        disconnect(mUiOnButton, SIGNAL(clicked(bool)), this, SLOT(uiButtonClicked(bool)));
        delete mUiOnButton;
    }
    
    delete mIconItem;
    delete mTotalImagesCount;
    delete mAlbumNameHeading;
    delete mMarkSelectHeading;
    delete mMarkCheckBox;
    delete mMarkCountLabel;
    delete mMarkingWidget;
    delete mZeroItemLabel;
    delete mToolBar;
    
    OstTraceFunctionExit0( DUP1_GLXGRIDVIEW_GLXGRIDVIEW_EXIT );
}

void GlxGridView::indicateLongPress(const QModelIndex& index, QPointF coords)
{
    OstTrace0( TRACE_NORMAL, GLXGRIDVIEW_INDICATELONGPRESS, "GlxGridView::indicateLongPress" );

     if ( mWidget->selectionMode() == HgWidget::MultiSelection )
         {
         return;
         }
     if ( mModel )
         {
         mModel->setData( index, index.row(), GlxFocusIndexRole );
         }
     emit itemSpecificMenuTriggered(viewId(),coords);
}

void GlxGridView::uiButtonClicked(bool /*checked*/)
{
    showHbItems();
}

void GlxGridView::showHbItems()
{
    if(mWidget && mWidget->selectionMode() == HgWidget::NoSelection) {
        setItemVisible( Hb::TitleBarItem, TRUE );
        setItemVisible( Hb::StatusBarItem, TRUE );
        setViewFlags(viewFlags() &~ HbView::ViewTitleBarHidden &~ HbView::ViewStatusBarHidden);
        showItemCount();
    }    
    updateToolBar();
    if (mUiOnButton) {
        mUiOnButton->hide();
    }
}

void GlxGridView::cameraButtonClicked(bool /*checked*/)
{
    emit actionTriggered(EGlxCmdCameraOpen);
}

int GlxGridView::getSubState()
{
    int substate = NO_GRID_S;
    if ( mModel ) {
        QVariant variant = mModel->data( mModel->index(0,0), GlxSubStateRole );
        if ( variant.isValid() &&  variant.canConvert<int> ()  ) {
            substate = variant.value<int>();
        }
    }
    return substate;
}

void GlxGridView::updateToolBar()
{
    //In the case of multiselection show the tool bar.
    if ( mWidget && mWidget->selectionMode() == HgWidget::MultiSelection ) {
        setItemVisible( Hb::ToolBarItem, TRUE ) ;
        return ;
    }
    
    //In the case of fetcher, there is no tool bar for album grid view
    int subState = getSubState();
    if ( subState == FETCHER_ALBUM_ITEM_S ) {
        setItemVisible( Hb::ToolBarItem, FALSE ) ;
    }
    else {
        setItemVisible( Hb::ToolBarItem, TRUE );
    }
}

void GlxGridView::createAlbumGridToolBar()
{
    mToolBar = new HbToolBar();
    mToolBar->setOrientation( Qt::Horizontal );
    mToolBar->setVisible(true);
    mToolBar->clearActions();
    
    HbAction *action = new HbAction();
    action->setData( EGlxCmdAddToAlbum );
    action->setIcon( HbIcon( GLXICON_ADD ) );
    action->setObjectName( "Add Action" );
    mToolBar->addAction( action );
    connect( action, SIGNAL( triggered( ) ), this, SLOT( handleToolBarAction( ) ) );
    
    action = new HbAction();
    action->setData( EGlxCmdRemoveFrom );
    action->setIcon( HbIcon( GLXICON_REMOVE ) );
    action->setObjectName( "Remove Action" );
    mToolBar->addAction( action );
    connect( action, SIGNAL( triggered( ) ), this, SLOT( handleToolBarAction( ) ) );
}

