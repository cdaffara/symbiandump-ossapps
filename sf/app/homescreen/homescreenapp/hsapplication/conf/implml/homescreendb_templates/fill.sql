INSERT INTO Scene (id) VALUES (1);

{% for index in range(feat_tree.HomeScreenDbSettings.Pages._value|length) -%}
    {%- set id = feat_tree.HomeScreenDbSettings.Pages.id._value[index] -%}
    {%- set indexPosition = feat_tree.HomeScreenDbSettings.Pages.indexPosition._value[index] -%}
    INSERT INTO Pages (id, indexPosition) VALUES ({{id}}, {{indexPosition}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.Widgets._value|length) -%}
    {%- set id = feat_tree.HomeScreenDbSettings.Widgets.id._value[index] -%}
    {%- set uri = feat_tree.HomeScreenDbSettings.Widgets.uri._value[index] or '' -%}
    {%- set pageId = feat_tree.HomeScreenDbSettings.Widgets.pageId._value[index] -%}
    
    INSERT INTO Widgets (id, uri, pageId) VALUES ({{id}}, '{{uri}}', {{pageId}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.WidgetPresentations._value|length) -%}
    {%- set key = feat_tree.HomeScreenDbSettings.WidgetPresentations.key._value[index] or '' -%}
    {%- set x = feat_tree.HomeScreenDbSettings.WidgetPresentations.x._value[index] -%}
    {%- set y = feat_tree.HomeScreenDbSettings.WidgetPresentations.y._value[index] -%}
    {%- set zValue = feat_tree.HomeScreenDbSettings.WidgetPresentations.zValue._value[index] -%}
    {%- set widgetId = feat_tree.HomeScreenDbSettings.WidgetPresentations.widgetId._value[index] -%}
    
    INSERT INTO WidgetPresentations (key, x, y, zValue, widgetId) VALUES ('{{key}}', {{x}}, {{y}}, {{zValue}}, {{widgetId}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.WidgetPreferences._value|length) -%}
    {%- set key = feat_tree.HomeScreenDbSettings.WidgetPreferences.key._value[index] or '' -%}
    {%- set value = feat_tree.HomeScreenDbSettings.WidgetPreferences.value._value[index] -%}
    {%- set widgetId = feat_tree.HomeScreenDbSettings.WidgetPreferences.widgetId._value[index] -%}
    
    INSERT INTO WidgetPreferences (key, value, widgetId) VALUES ('{{key}}', '{{value}}', {{widgetId}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.Configuration._value|length) -%}
    {%- set key = feat_tree.HomeScreenDbSettings.Configuration.key._value[index] or '' -%}
    {%- set value = feat_tree.HomeScreenDbSettings.Configuration.value._value[index] -%}
    
    INSERT INTO Configuration (key, value) VALUES ('{{key}}', '{{value}}');
{% endfor %}
