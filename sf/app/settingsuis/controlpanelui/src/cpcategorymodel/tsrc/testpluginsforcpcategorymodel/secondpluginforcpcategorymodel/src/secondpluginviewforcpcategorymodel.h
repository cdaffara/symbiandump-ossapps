#ifndef	SECONDPLUGINVIEWFORCPCATEGORYMODEL_H
#define	SECONDPLUGINVIEWFORCPCATEGORYMODEL_H

#include <cpbasesettingview.h>
#include <QGraphicsItem>

class HbDataFormModelItem;
class CpSettingFormItemData;

class SecondPluginViewForCategoryModel : public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit SecondPluginViewForCategoryModel(QGraphicsItem *parent = 0);
    virtual ~SecondPluginViewForCategoryModel();
public:
    void testClose();
private slots:   
    void sliderValueChanged(int value);
    
private:
    HbDataFormModelItem   *mGroupItem;
    CpSettingFormItemData *mSliderItem;
};
#endif
