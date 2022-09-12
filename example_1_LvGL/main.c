#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "existosapi/basic_api.h"
#include "existosapi/keyboard_gii39.h"
#include "existosapi/syscall.h"
#include "existosapi/SystemUI.h"

#include "FreeRTOS.h"
#include "task.h"

extern const lv_font_t SourceHanSans11; //中文字体

static lv_obj_t *screen;
static lv_group_t *group_default_backup; //用于备份System界面输入设备绑定的Group，返回时需恢复Group

static lv_obj_t *win;
static lv_obj_t *cont;
static lv_group_t *group_win;    //窗口组件使用的group
static lv_group_t *group_msgbox; //信息框窗口使用的group
static lv_group_t *group_backup; //弹出新窗口时备份旧窗口的组

static bool app_running = true;

lv_style_t label_style; //对象的style信息（如颜色、边框、字体等）

static const char *mbox_btns[] = {"确认", "取消", ""};

static void msg_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *msgbox = lv_event_get_current_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t select_btn = lv_msgbox_get_active_btn(msgbox); //取出先择了信息框的第几个按钮，0,1,2
        printf("Select:%d\n", select_btn);
        lv_group_set_default(group_backup);
        lv_indev_set_group(SystemGetInKeypad(), group_backup); //还原输入设备绑定为上一窗口
        lv_msgbox_close_async(msgbox);  //在信息框回调函数内关闭信息框需要调用异步销毁信息框
    }
}

static void app_btn_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_CLICKED)
    {
        lv_obj_t *label = lv_obj_get_child(obj, 0);
        if (strcmp(lv_label_get_text(label), "信息框") == 0)
        {
            group_backup = SystemGetInKeypad()->group; //备份当前输入设备所绑定的组
            lv_group_remove_all_objs(group_msgbox);
            lv_group_set_default(group_msgbox); //设定新创建的窗口归入指定的组
            lv_obj_t *obj = lv_msgbox_create(screen, "信息框", "测试信息。", (const char **)mbox_btns, false);
            lv_obj_add_style(obj, &label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_add_event_cb(obj, msg_cb, LV_EVENT_ALL, NULL);
            lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
            lv_obj_center(obj);
            lv_indev_set_group(SystemGetInKeypad(), group_msgbox);
        }
        if (strcmp(lv_label_get_text(label), "退出") == 0)
        {
            app_running = false;
        }
    }
}

int main()
{
    ll_cpu_slowdown_enable(false); //关闭CPU自动降频

    lv_obj_invalidate(lv_scr_act());
    SystemUIResume(); // app启动时系统自动暂停了LvGL，通过此api恢复

    screen = lv_scr_act();                         //获取当前屏幕对象
    group_default_backup = lv_group_get_default(); //备份进入应用前系统正在使用的组

    win = lv_win_create(screen, 0);   //在当前屏幕上创建一个无标题栏的窗口
    cont = lv_win_get_content(win);   //获取窗口内容对象
    group_win = lv_group_create();    //创建一个当前应用界面所使用的Group
    group_msgbox = lv_group_create(); //创建一个信息框所使用的Group

    lv_group_set_default(group_win);                    //将默认Group设置为当前app创建的group，之后创建对象时会自动将对象加入该组
    lv_indev_set_group(SystemGetInKeypad(), group_win); //获取系统键盘设备，并将键盘绑定的到当前窗口的对象组

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN); //设置窗口使用flex布局，新组件按列从上到下生成

    lv_style_init(&label_style);                            //初始化一个style
    lv_style_set_text_font(&label_style, &SourceHanSans11); //设置style的字体

    lv_obj_t *label = lv_label_create(cont); //创建一个标签

    lv_obj_add_style(label, &label_style, LV_PART_MAIN | LV_STATE_DEFAULT); //给标签应用风格
    lv_label_set_text(label, "中文标签测试");                               //设置标签名

    lv_obj_t *obj;
    obj = lv_btn_create(cont); //创建一个按钮
    lv_obj_add_event_cb(obj,
                        app_btn_handler,
                        LV_EVENT_ALL,
                        NULL);  //添加按钮事件回调函数
    obj = lv_label_create(obj); //在按钮中创建标签作为按钮显示名
    lv_label_set_text(obj, "信息框");
    lv_obj_add_style(obj, &label_style, LV_PART_MAIN | LV_STATE_DEFAULT); //给标签应用风格

    obj = lv_btn_create(cont);
    lv_obj_add_event_cb(obj, app_btn_handler, LV_EVENT_ALL, NULL);
    obj = lv_label_create(obj);
    lv_label_set_text(obj, "退出");
    lv_obj_add_style(obj, &label_style, LV_PART_MAIN | LV_STATE_DEFAULT); //给标签应用风格

    while (app_running) //窗口创建和各种UI组件已创建完毕，此处空转等待结束（或处理其它信息）
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    lv_group_set_default(group_default_backup);                    //还原为系统默认组
    lv_indev_set_group(SystemGetInKeypad(), group_default_backup); //将输入设备归还给系统
    lv_obj_del(win);                                               //销毁窗口
    lv_group_del(group_win);
    return 0;
}
