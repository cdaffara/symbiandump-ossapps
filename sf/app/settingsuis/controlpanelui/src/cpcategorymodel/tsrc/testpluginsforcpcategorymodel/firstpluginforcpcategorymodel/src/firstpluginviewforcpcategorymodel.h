#ifndef	FIRSTPLUGINVIEWFORCPCATEGORYMODEL_H
#define	FIRSTPLUGINVIEWFORCPCATEGORYMODEL_H

#include <cpbasesettingview.h>
#include <QGraphicsItem>

class HbDataFormModelItem;
class CpSettingFormItemData;

class FirstPluginViewForCategoryModel : public CpBaseSettingView
{
    Q_OBJECT
public:
    explicit FirstPluginViewForCategoryModel(QGraphicsItem *parent = 0);
    virtual ~FirstPluginViewForCategoryModel();
public:
    void testClose();
private slots:   
    void sliderValueChanged(int value);
    
private:
    HbDataFormModelItem   *mGroupItem;
    CpSettingFormItemData *mSliderItem;
};
#endif
