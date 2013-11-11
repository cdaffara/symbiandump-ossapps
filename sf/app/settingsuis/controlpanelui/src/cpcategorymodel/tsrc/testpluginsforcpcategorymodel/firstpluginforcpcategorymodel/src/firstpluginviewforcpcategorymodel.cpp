#include "firstpluginviewforcpcategorymodel.h"

#include <hbdataform.h>
#include <qstringlist>
#include <QDebug>
#include <hbdataformmodel.h>
#include <cpsettingformitemdata.h>
#include <hbmessagebox.h>

FirstPluginViewForCategoryModel::FirstPluginViewForCategoryModel(QGraphicsItem *parent) 
    : CpBaseSettingView(0,parent),
    mGroupItem(0),
    mSliderItem(0)
{
    HbDataForm *form = qobject_cast<HbDataForm*>(widget());
    if (form) {
        HbDataFormModel *model = new HbDataFormModel;
        
        form->setHeading(tr("View from test plugin"));
        mGroupItem = new HbDataFormModelItem(HbDataFormModelItem::GroupItem, QString("Group"));
        
        model->appendDataFormItem(mGroupItem);
        
        mSliderItem = new CpSettingFormItemData(HbDataFormModelItem::SliderItem,
                QString("New Slider"));
        mSliderItem->setContentWidgetData("iconEnabled","FALSE");
        form->addConnection(mSliderItem,SIGNAL(valueChanged(int)),this,SLOT(sliderValueChanged(int)));
        mGroupItem->appendChild(mSliderItem);      
        
        form->setModel(model);
    }
}
FirstPluginViewForCategoryModel::~FirstPluginViewForCategoryModel()
{
}

void FirstPluginViewForCategoryModel::testClose()
{
    close();
}
void FirstPluginViewForCategoryModel::sliderValueChanged(int value)
{
    //Disable the slider alert.
	HbMessageBox::information(QString("slider value changed to:%1").arg(value));
}
