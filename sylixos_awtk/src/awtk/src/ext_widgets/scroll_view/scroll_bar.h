﻿/**
 * File:   scroll_bar.h
 * Author: AWTK Develop Team
 * Brief:  scroll_bar
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-07-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_SCROLL_BAR_H
#define TK_SCROLL_BAR_H

#include "base/widget.h"
#include "base/widget_animator.h"

BEGIN_C_DECLS

/**
 * @class scroll_bar_t
 * @parent widget_t
 * @annotation ["scriptable"]
 * scroll_bar控件。
 */
typedef struct _scroll_bar_t {
  widget_t widget;
  /**
   * @property {int32_t} virtual_size
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 虚拟宽度或高度。
   */
  int32_t virtual_size;
  /**
   * @property {int32_t} value
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 当前的值。
   */
  int32_t value;
  /**
   * @property {int32_t} row
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 行的高度。
   */
  int32_t row;
  /**
   * @property {bool_t} animatable
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 滚动时是否启用动画。
   */
  bool_t animatable;

  /*private*/
  widget_t* dragger;
  widget_animator_t* wa_value;
  widget_animator_t* wa_opactiy;
} scroll_bar_t;

/**
 * @method scroll_bar_create
 * 创建scroll_bar对象(根据宏WITH_DESKTOP_STYLE决定创建desktop风格还是mobile风格的滚动条)
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* scroll_bar_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method scroll_bar_cast
 * 转换为scroll_bar对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget scroll_bar对象。
 *
 * @return {widget_t*} scroll_bar对象。
 */
widget_t* scroll_bar_cast(widget_t* widget);

/**
 * @method scroll_bar_create_mobile
 * 创建mobile风格的scroll_bar对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* scroll_bar_create_mobile(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method scroll_bar_create_desktop
 * 创建desktop风格的scroll_bar对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* scroll_bar_create_desktop(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method scroll_bar_set_params
 * 设置参数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} virtual_size 最大值。
 * @param {int32_t} row 每一行的高度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_params(widget_t* widget, int32_t virtual_size, int32_t row);

/**
 * @method scroll_bar_scroll_to
 * 滚动到指定的值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} value 值。
 * @param {int32_t} duration 时间。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_scroll_to(widget_t* widget, int32_t value, int32_t duration);

/**
 * @method scroll_bar_set_value
 * 设置值，并触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_value(widget_t* widget, int32_t value);

/**
 * @method scroll_bar_add_delta
 * 在当前的值上增加一个值，并触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} delta 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_add_delta(widget_t* widget, int32_t delta);

/**
 * @method scroll_bar_scroll_delta
 * 在当前的值上增加一个值，并滚动到新的值，并触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} delta 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_scroll_delta(widget_t* widget, int32_t delta);

/**
 * @method scroll_bar_set_value_only
 * 设置值，但不触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_value_only(widget_t* widget, int32_t value);

/**
 * @method scroll_bar_is_mobile
 * 判断是否是mobile风格的滚动条。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 *
 * @return {bool_t} 返回TRUE表示是mobile风格的，否则表示不是mobile风格的。
 */
bool_t scroll_bar_is_mobile(widget_t* widget);

#define SCROLL_BAR(widget) ((scroll_bar_t*)(widget))

END_C_DECLS

#endif /*TK_SCROLL_BAR_H*/
