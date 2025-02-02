/**
 * File:   window_manager.c
 * Author: AWTK Develop Team
 * Brief:  window manager
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
 * 2018-01-13 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "base/keys.h"
#include "base/mem.h"
#include "base/idle.h"
#include "base/time.h"
#include "base/utils.h"
#include "base/timer.h"
#include "base/layout.h"
#include "base/locale_info.h"
#include "base/system_info.h"
#include "base/window_manager.h"

static ret_t window_manager_inc_fps(widget_t* widget);
static ret_t window_manager_update_fps(widget_t* widget);
static ret_t window_manager_do_open_window(widget_t* wm, widget_t* window);
static ret_t window_manger_layout_child(widget_t* widget, widget_t* window);

static widget_t* window_manager_find_prev_window(widget_t* widget) {
  int32_t i = 0;
  int32_t nr = 0;
  return_value_if_fail(widget != NULL, NULL);

  if (widget->children != NULL && widget->children->size > 0) {
    nr = widget->children->size;
    for (i = nr - 2; i >= 0; i--) {
      widget_t* iter = (widget_t*)(widget->children->elms[i]);
      const char* type = widget_get_type(iter);
      if (tk_str_eq(type, WIDGET_TYPE_NORMAL_WINDOW)) {
        return iter;
      }
    }
  }

  return NULL;
}

static ret_t window_manager_create_animator(window_manager_t* wm, widget_t* curr_win, bool_t open) {
  value_t v;
  const char* anim_hint = NULL;
  widget_t* prev_win = window_manager_find_prev_window(WIDGET(wm));
  const char* key = open ? WIDGET_PROP_OPEN_ANIM_HINT : WIDGET_PROP_CLOSE_ANIM_HINT;

  return_value_if_fail(wm != NULL && prev_win != NULL && curr_win != NULL, RET_BAD_PARAMS);

  if (wm->animator != NULL) {
    return RET_FAIL;
  }

  if (widget_get_prop(curr_win, key, &v) == RET_OK) {
    anim_hint = value_str(&(v));
  } else {
    key = WIDGET_PROP_ANIM_HINT;
    if (widget_get_prop(curr_win, key, &v) == RET_OK) {
      anim_hint = value_str(&(v));
    }
  }

  if (anim_hint && *anim_hint) {
    if (open) {
      wm->animator = window_animator_create_for_open(anim_hint, wm->canvas, prev_win, curr_win);
    } else {
      wm->animator = window_animator_create_for_close(anim_hint, wm->canvas, prev_win, curr_win);
    }

    wm->animating = wm->animator != NULL;
    if (wm->animating) {
      wm->ignore_user_input = TRUE;
      log_debug("ignore_user_input\n");
    }
  }

  return wm->animating ? RET_OK : RET_FAIL;
}

static ret_t on_idle_invalidate(const timer_info_t* info) {
  widget_t* curr_win = WIDGET(info->ctx);
  widget_invalidate_force(curr_win);

  return RET_REMOVE;
}

static ret_t window_manager_check_if_need_open_animation(const idle_info_t* info) {
  widget_t* curr_win = WIDGET(info->ctx);
  window_manager_t* wm = WINDOW_MANAGER(curr_win->parent);
  event_t e = event_init(EVT_WINDOW_WILL_OPEN, curr_win);

  widget_dispatch(curr_win, &e);
  if (window_manager_create_animator(wm, curr_win, TRUE) != RET_OK) {
    e = event_init(EVT_WINDOW_OPEN, curr_win);
    widget_dispatch(curr_win, &e);

    timer_add(on_idle_invalidate, curr_win, 100);
  }

  return RET_REMOVE;
}

static ret_t window_manager_idle_dispatch_window_open(const idle_info_t* info) {
  widget_t* curr_win = WIDGET(info->ctx);

  event_t e = event_init(EVT_WINDOW_WILL_OPEN, curr_win);
  widget_dispatch(curr_win, &e);

  e = event_init(EVT_WINDOW_OPEN, curr_win);
  widget_dispatch(curr_win, &e);

  return RET_REMOVE;
}

static ret_t window_manager_check_if_need_close_animation(window_manager_t* wm,
                                                          widget_t* curr_win) {
  return window_manager_create_animator(wm, curr_win, FALSE);
}

static ret_t window_manager_do_open_window(widget_t* widget, widget_t* window) {
  if (widget->children != NULL && widget->children->size > 0) {
    idle_add((idle_func_t)window_manager_check_if_need_open_animation, window);
  } else {
    idle_add((idle_func_t)window_manager_idle_dispatch_window_open, window);
  }

  return RET_OK;
}

ret_t window_manager_open_window(widget_t* widget, widget_t* window) {
  ret_t ret = RET_OK;
  window_manager_t* wm = WINDOW_MANAGER(widget);
  return_value_if_fail(widget != NULL && window != NULL, RET_BAD_PARAMS);

  if (wm->animator != NULL) {
    wm->pending_open_window = window;
  } else {
    window_manager_do_open_window(widget, window);
  }

  ret = widget_add_child(widget, window);
  window_manger_layout_child(widget, window);

  if (ret == RET_OK) {
    widget->target = window;
  }

  window->dirty = FALSE;
  widget_invalidate(window, NULL);

  return ret;
}

static ret_t window_manager_idle_destroy_window(const idle_info_t* info) {
  widget_t* win = WIDGET(info->ctx);
  widget_destroy(win);

#ifdef ENABLE_MEM_LEAK_CHECK
  tk_mem_dump();
#endif /*ENABLE_MEM_LEAK_CHECK*/

  return RET_OK;
}

ret_t window_manager_close_window(widget_t* widget, widget_t* window) {
  ret_t ret = RET_OK;
  window_manager_t* wm = WINDOW_MANAGER(widget);
  event_t e = event_init(EVT_WINDOW_CLOSE, window);
  return_value_if_fail(widget != NULL && window != NULL, RET_BAD_PARAMS);

  if (widget->target == window) {
    widget->target = NULL;
  }
  if (widget->key_target == window) {
    widget->key_target = NULL;
  }
  if (widget->grab_widget != NULL) {
    if (widget->grab_widget == window) {
      widget->grab_widget = NULL;
    }
  }

  if (wm->animator) {
    wm->pending_close_window = window;
    return RET_OK;
  }

  widget_dispatch(window, &e);
  if (window_manager_check_if_need_close_animation(wm, window) != RET_OK) {
    widget_remove_child(widget, window);
    idle_add(window_manager_idle_destroy_window, window);
  }

  return ret;
}

widget_t* window_manager_find_target(widget_t* widget, xy_t x, xy_t y) {
  point_t p = {x, y};
  return_value_if_fail(widget != NULL, NULL);

  if (widget->grab_widget != NULL) {
    return widget->grab_widget;
  }

  widget_to_local(widget, &p);
  WIDGET_FOR_EACH_CHILD_BEGIN_R(widget, iter, i)
  xy_t r = iter->x + iter->w;
  xy_t b = iter->y + iter->h;
  const char* type = widget_get_type(iter);

  if (p.x >= iter->x && p.y >= iter->y && p.x <= r && p.y <= b) {
    return iter;
  }

  if (tk_str_eq(type, WIDGET_TYPE_NORMAL_WINDOW) || tk_str_eq(type, WIDGET_TYPE_DIALOG) ||
      tk_str_eq(type, WIDGET_TYPE_POPUP)) {
    return iter;
  }
  WIDGET_FOR_EACH_CHILD_END()

  return NULL;
}

static rect_t window_manager_calc_dirty_rect(window_manager_t* wm) {
  rect_t r = wm->dirty_rect;
  widget_t* widget = WIDGET(wm);
  rect_t* ldr = &(wm->last_dirty_rect);

  rect_merge(&r, ldr);

  return rect_fix(&r, widget->w, widget->h);
}

static ret_t window_manager_paint_normal(widget_t* widget, canvas_t* c) {
  window_manager_t* wm = WINDOW_MANAGER(widget);
  rect_t* dr = &(wm->dirty_rect);

  window_manager_inc_fps(widget);

  if ((dr->w && dr->h) || wm->show_fps) {
    uint32_t start_time = time_now_ms();
    rect_t r = window_manager_calc_dirty_rect(wm);

    if ((r.w > 0 && r.h > 0) || wm->show_fps) {
      ENSURE(canvas_begin_frame(c, &r, LCD_DRAW_NORMAL) == RET_OK);
      ENSURE(widget_paint(WIDGET(wm), c) == RET_OK);
      ENSURE(canvas_end_frame(c) == RET_OK);
      wm->last_paint_cost = time_now_ms() - start_time;
      /*
        log_debug("%s x=%d y=%d w=%d h=%d cost=%d\n", __FUNCTION__, (int)(r.x), (int)(r.y),
                (int)(r.w), (int)(r.h), (int)wm->last_paint_cost);
      */
    }
  }

  wm->last_dirty_rect = wm->dirty_rect;
  wm->dirty_rect = rect_init(widget->w, widget->h, 0, 0);

  return RET_OK;
}

static ret_t window_manager_paint_animation(widget_t* widget, canvas_t* c) {
  uint32_t start_time = time_now_ms();
  window_manager_t* wm = WINDOW_MANAGER(widget);

  ret_t ret = window_animator_update(wm->animator, start_time);
  wm->last_paint_cost = time_now_ms() - start_time;
  window_manager_inc_fps(widget);
  /*
  log_debug("%s cost=%d\n", __FUNCTION__, (int)wm->last_paint_cost);
  */
  if (ret == RET_DONE) {
    if (wm->animator->open) {
      widget_t* curr_win = wm->animator->curr_win;
      event_t e = event_init(EVT_WINDOW_OPEN, curr_win);
      widget_dispatch(curr_win, &e);
    }

    window_animator_destroy(wm->animator);
    wm->animator = NULL;
    wm->animating = FALSE;
    wm->ignore_user_input = FALSE;

    if (wm->pending_close_window != NULL) {
      widget_t* window = wm->pending_close_window;
      wm->pending_close_window = NULL;
      window_manager_close_window(widget, window);
    } else if (wm->pending_open_window != NULL) {
      widget_t* window = wm->pending_open_window;
      wm->pending_open_window = NULL;
      window_manager_do_open_window(widget, window);
    }
  }

  return RET_OK;
}

static ret_t window_manager_inc_fps(widget_t* widget) {
  window_manager_t* wm = WINDOW_MANAGER(widget);

  wm->fps_count++;

  return RET_OK;
}

static ret_t window_manager_update_fps(widget_t* widget) {
  uint32_t elapse = 0;
  uint32_t now = time_now_ms();
  window_manager_t* wm = WINDOW_MANAGER(widget);

  elapse = now - wm->fps_time;
  if (elapse >= 200) {
    wm->fps = wm->fps_count * 1000 / elapse;

    wm->fps_time = now;
    wm->fps_count = 0;
  }

  canvas_set_fps(wm->canvas, wm->show_fps, wm->fps);

  return RET_OK;
}

ret_t window_manager_paint(widget_t* widget, canvas_t* c) {
  ret_t ret = RET_OK;
  window_manager_t* wm = WINDOW_MANAGER(widget);
  return_value_if_fail(wm != NULL && c != NULL, RET_BAD_PARAMS);

  wm->canvas = c;
  canvas_set_global_alpha(c, 0xff);
  window_manager_update_fps(widget);

  if (wm->animator != NULL) {
    ret = window_manager_paint_animation(widget, c);
  } else {
    ret = window_manager_paint_normal(widget, c);
  }

  return ret;
}

static widget_t* s_window_manager = NULL;

widget_t* window_manager(void) {
  return s_window_manager;
}

ret_t window_manager_set(widget_t* widget) {
  s_window_manager = widget;

  return RET_OK;
}

widget_t* window_manager_create(void) {
  window_manager_t* wm = TKMEM_ZALLOC(window_manager_t);
  return_value_if_fail(wm != NULL, NULL);

  return window_manager_init(wm);
}

static ret_t window_manager_invalidate(widget_t* widget, rect_t* r) {
  window_manager_t* wm = WINDOW_MANAGER(widget);
  rect_t* dr = &(wm->dirty_rect);

  rect_merge(dr, r);

  return RET_OK;
}

int32_t window_manager_find_top_window_index(widget_t* widget) {
  return_value_if_fail(widget != NULL, -1);

  WIDGET_FOR_EACH_CHILD_BEGIN_R(widget, iter, i)
  const char* type = widget_get_type(iter);
  if (tk_str_eq(type, WIDGET_TYPE_NORMAL_WINDOW)) {
    return i;
  }
  WIDGET_FOR_EACH_CHILD_END();

  return -1;
}

widget_t* window_manager_get_top_window(widget_t* widget) {
  int32_t index = window_manager_find_top_window_index(widget);
  return_value_if_fail(index >= 0, NULL);

  return widget_get_child(widget, index);
}

ret_t window_manager_on_paint_children(widget_t* widget, canvas_t* c) {
  int32_t start = 0;
  return_value_if_fail(widget != NULL && c != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN_R(widget, iter, i)
  const char* type = widget_get_type(iter);
  if (tk_str_eq(type, WIDGET_TYPE_NORMAL_WINDOW)) {
    start = i;
    break;
  }
  WIDGET_FOR_EACH_CHILD_END()

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  if (i >= start && iter->visible) {
    widget_paint(iter, c);
  }
  WIDGET_FOR_EACH_CHILD_END()

  return RET_OK;
}

static ret_t wm_on_remove_child(widget_t* widget, widget_t* window) {
  widget_t* top = window_manager_get_top_window(widget);

  if (top != NULL) {
    rect_t r;
    r = rect_init(window->x, window->y, window->w, window->h);
    widget_invalidate(top, &r);
  }

  return RET_FAIL;
}

static const widget_vtable_t s_window_manager_vtable = {
    .type = WIDGET_TYPE_WINDOW_MANAGER,
    .invalidate = window_manager_invalidate,
    .on_paint_children = window_manager_on_paint_children,
    .on_remove_child = wm_on_remove_child,
    .find_target = window_manager_find_target,
};

static ret_t wm_on_locale_changed(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  widget_re_translate_text(iter);
  widget_dispatch(iter, e);
  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

widget_t* window_manager_init(window_manager_t* wm) {
  widget_t* w = &(wm->widget);
  return_value_if_fail(wm != NULL, NULL);

  widget_init(w, NULL, &s_window_manager_vtable, 0, 0, 0, 0);

  locale_info_on(locale_info(), EVT_LOCALE_CHANGED, wm_on_locale_changed, wm);

  return w;
}

static ret_t window_manger_layout_child(widget_t* widget, widget_t* window) {
  xy_t x = window->x;
  xy_t y = window->y;
  wh_t w = window->w;
  wh_t h = window->h;
  const char* type = widget_get_type(window);

  if (tk_str_eq(type, WIDGET_TYPE_NORMAL_WINDOW)) {
    x = 0;
    y = 0;
    w = widget->w;
    h = widget->h;
  } else if (tk_str_eq(type, WIDGET_TYPE_DIALOG)) {
    x = (widget->w - window->w) >> 1;
    y = (widget->h - window->h) >> 1;
  }

  widget_move_resize(window, x, y, w, h);
  widget_layout(window);

  return RET_OK;
}

static ret_t window_manger_layout_children(widget_t* widget) {
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  WIDGET_FOR_EACH_CHILD_BEGIN(widget, iter, i)
  window_manger_layout_child(widget, iter);
  WIDGET_FOR_EACH_CHILD_END();

  return RET_OK;
}

ret_t window_manager_resize(widget_t* widget, wh_t w, wh_t h) {
  window_manager_t* wm = WINDOW_MANAGER(widget);
  return_value_if_fail(wm != NULL, RET_BAD_PARAMS);

  wm->dirty_rect.x = 0;
  wm->dirty_rect.y = 0;
  wm->dirty_rect.w = w;
  wm->dirty_rect.h = h;
  wm->last_dirty_rect = wm->dirty_rect;
  widget_move_resize(widget, 0, 0, w, h);

  return window_manger_layout_children(widget);
}

static ret_t window_manager_update_key_status(window_manager_t* wm, uint32_t key, bool_t down) {
  if (key == FKEY_LSHIFT || key == FKEY_RSHIFT) {
    wm->shift = down;
  }
  if (key == FKEY_LALT || key == FKEY_RALT) {
    wm->alt = down;
  }
  if (key == FKEY_LCTRL || key == FKEY_RCTRL) {
    wm->ctrl = down;
  }
  if (key == FKEY_CAPSLOCK) {
    wm->caplock = down;
  }

  return RET_OK;
}

typedef struct _key_shift_t {
  char key;
  char shift_key;
} key_shift_t;

static const key_shift_t key_shift[] = {
    {'`', '~'}, {'1', '!'}, {'2', '@'}, {'3', '#'},  {'4', '$'}, {'5', '%'}, {'6', '^'},
    {'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'},  {'-', '_'}, {'=', '+'}, {'[', '{'},
    {']', '}'}, {',', '<'}, {'.', '>'}, {'\\', '|'}, {'/', '?'},
};

static ret_t window_manager_shift_key(window_manager_t* wm, key_event_t* e) {
  char c = (char)e->key;
  if (wm->shift) {
    uint32_t i = 0;
    for (i = 0; i < ARRAY_SIZE(key_shift); i++) {
      if (key_shift[i].key == c) {
        e->key = key_shift[i].shift_key;
        return RET_OK;
      }
    }
  }

  if (wm->shift && wm->caplock) {
    return RET_OK;
  }

  if (wm->shift || wm->caplock) {
    if (c >= FKEY_a && c <= FKEY_z) {
      e->key = c - 32;
    }
  }

  return RET_OK;
}

ret_t window_manager_dispatch_input_event(widget_t* widget, event_t* e) {
  window_manager_t* wm = WINDOW_MANAGER(widget);
  return_value_if_fail(wm != NULL && e != NULL, RET_BAD_PARAMS);

  if (wm->ignore_user_input) {
    if (wm->pressed && e->type == EVT_POINTER_UP) {
      log_debug("animating ignore input, but it is last pointer_up\n");
    } else {
      log_debug("animating ignore input\n");
      return RET_OK;
    }
  }

  switch (e->type) {
    case EVT_POINTER_DOWN: {
      pointer_event_t* evt = (pointer_event_t*)e;
      pointer_event_rotate(evt, system_info());

      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      wm->pressed = TRUE;
      widget_on_pointer_down(widget, evt);
      break;
    }
    case EVT_POINTER_MOVE: {
      pointer_event_t* evt = (pointer_event_t*)e;
      pointer_event_rotate(evt, system_info());

      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      widget_on_pointer_move(widget, evt);
      break;
    }
    case EVT_POINTER_UP: {
      pointer_event_t* evt = (pointer_event_t*)e;
      pointer_event_rotate(evt, system_info());

      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      widget_on_pointer_up(widget, evt);
      wm->pressed = FALSE;
      break;
    }
    case EVT_KEY_DOWN: {
      key_event_t* evt = (key_event_t*)e;
      window_manager_update_key_status(wm, evt->key, TRUE);
      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      evt->caplock = wm->caplock;

      window_manager_shift_key(wm, evt);
      widget_on_keydown(widget, evt);
      break;
    }
    case EVT_KEY_UP: {
      key_event_t* evt = (key_event_t*)e;

      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      evt->caplock = wm->caplock;

      window_manager_shift_key(wm, evt);
      widget_on_keyup(widget, evt);

      window_manager_update_key_status(wm, evt->key, FALSE);
      break;
    }
    case EVT_CONTEXT_MENU: {
      pointer_event_t* evt = (pointer_event_t*)e;
      pointer_event_rotate(evt, system_info());

      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      widget_dispatch_to_target(widget, e);
      break;
    }
    case EVT_WHEEL: {
      wheel_event_t* evt = (wheel_event_t*)e;

      evt->alt = wm->alt;
      evt->ctrl = wm->ctrl;
      evt->shift = wm->shift;
      widget_dispatch_to_key_target(widget, e);
      break;
    }
    default:
      break;
  }

  return RET_OK;
}

ret_t window_manager_set_animating(widget_t* widget, bool_t animating) {
  window_manager_t* wm = WINDOW_MANAGER(widget);
  return_value_if_fail(wm != NULL, RET_BAD_PARAMS);

  wm->animating = animating;

  return RET_OK;
}

ret_t window_manager_set_show_fps(widget_t* widget, bool_t show_fps) {
  window_manager_t* wm = WINDOW_MANAGER(widget);
  return_value_if_fail(wm != NULL, RET_BAD_PARAMS);

  wm->show_fps = show_fps;

  return RET_OK;
}

widget_t* window_manager_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_window_manager_vtable, NULL);

  return widget;
}
