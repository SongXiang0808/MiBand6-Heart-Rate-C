#include <stdio.h>      // 标准输入输出库
#include <errno.h>      // 错误号定义
#include <ctype.h>      // 字符处理库
#include <fcntl.h>      // 文件控制定义
#include <unistd.h>     // UNIX 标准函数定义
#include <stdlib.h>     // 标准库函数
#include <string.h>     // 字符串处理库
#include <sys/param.h>  // 系统参数
#include <sys/socket.h> // 套接字库
#include "bluetooth/bluetooth.h"  // 蓝牙库
#include "bluetooth/hci.h"        // HCI（主机控制接口）库
#include "bluetooth/hci_lib.h"    // HCI 库函数

// 如果没有定义 MIN 宏，则定义 MIN 宏，用于返回两个值中的较小值
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* 非官方值，可能会更改 */
#define LE_LINK 0x80

// 广告数据类型标志
#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

// 扩展询问响应（EIR）数据类型
#define EIR_NAME_SHORT 0x08  /* 缩短的本地名称 */
#define EIR_NAME_COMPLETE 0x09  /* 完整的本地名称 */
#define EIR_MANUFACTURER_SPECIFIC 0xFF  /* 制造商特定数据 */

// HCI 最大事件大小
#define HCI_MAX_EVENT_SIZE 260

// 解析 EIR 数据中的名称
static int eir_parse_name(uint8_t *eir, size_t eir_len, char *buf, size_t buf_len)
{
    size_t offset = 0;
    while (offset < eir_len) {
        uint8_t field_len = eir[offset];
        size_t name_len;
        // 检查 EIR 数据的结束
        if (field_len == 0)
            break;
        if (offset + field_len > eir_len)
            goto failed;
        switch (eir[offset + 1]) {
            case EIR_NAME_SHORT:
            case EIR_NAME_COMPLETE:
                name_len = field_len - 1; 
                if (name_len > buf_len)
                    goto failed;
                memcpy(buf, &eir[offset + 2], name_len);
                buf[name_len] = '\0'; // 确保字符串以 null 结尾
                return 0;
            case EIR_MANUFACTURER_SPECIFIC:
                // 检查制造商特定数据是否为心率数据
                if (field_len >= 7 && eir[offset + 2] == 0x57 && eir[offset + 3] == 0x01 &&
                    eir[offset + 4] == 0x02 && eir[offset + 5] == 0x02 && eir[offset + 6] == 0x01) {
                    printf("Heart Rate: %d\n", eir[offset + 7]);
                    return 1; // 找到心率数据
                }
                break;
        }
        offset += field_len + 1; 
    }
failed:
    snprintf(buf, buf_len, "(unknown)");
    return 0;
}

// 打印广告设备信息
static int print_advertising_devices(int dd)
{
    unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
    struct hci_filter nf, of;
    socklen_t olen;
    int len;
    // 获取当前套接字选项
    olen = sizeof(of);
    if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
        printf("Could not get socket options\n");
        return -1;
    }
    // 设置新的 HCI 过滤器
    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);       // 设置数据包类型为事件数据包
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);   // 设置事件类型为 LE 元事件
    if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
        printf("Could not set socket options\n");
        return -1;
    }
    // 循环读取广告数据
    while (1) {
        evt_le_meta_event *meta;
        le_advertising_info *info;
        char addr[18];
        // 读取数据
        while ((len = read(dd, buf, sizeof(buf))) < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            goto done;
        }
        // 解析数据
        ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
        len -= (1 + HCI_EVENT_HDR_SIZE);
        meta = (evt_le_meta_event *) ptr;
        if (meta->subevent != 0x02)
            goto done;
        // 忽略多个报告
        info = (le_advertising_info *) (meta->data + 1);
        {
            char name[248];
            memset(name, 0, sizeof(name));
            ba2str(&info->bdaddr, addr);
            if (eir_parse_name(info->data, info->length, name, sizeof(name) - 1)) {
                printf("%s %s\n", addr, name);
            }
        }
    }
done:
    // 恢复原始套接字选项
    setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));
    if (len < 0)
        return -1;
    return 0;
}

// 执行 LE 扫描
int lescan(int dev_id, int argc, char **argv)
{
    int err, dd;
    uint8_t own_type = LE_PUBLIC_ADDRESS;
    uint8_t scan_type = 0x00;
    uint8_t filter_policy = 0x00;
    uint16_t interval = htobs(0x0010);
    uint16_t window = htobs(0x0010);
    uint8_t filter_dup = 0x00; // 禁用重复过滤

    // 打开 HCI 设备
    dd = hci_open_dev(dev_id);
    if (dd < 0) {
        perror("Could not open device");
        exit(1);
    }

    // 设置扫描参数
    err = hci_le_set_scan_parameters(dd, scan_type, interval, window,
                                     own_type, filter_policy, 10000);
    if (err < 0) {
        perror("Set scan parameters failed");
        exit(1);
    }

    // 启用扫描
    err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 10000);
    if (err < 0) {
        perror("Enable scan failed");
        exit(1);
    }

    printf("LE Scan ...\n");

    // 打印广告设备信息
    err = print_advertising_devices(dd);
    if (err < 0) {
        perror("Could not receive advertising events");
        exit(1);
    }

    // 禁用扫描
    err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 10000);
    if (err < 0) {
        perror("Disable scan failed");
        exit(1);
    }

    // 关闭 HCI 设备
    hci_close_dev(dd);
    return 0;
}

// 主函数
int main(int argc, char **argv)
{
    // 重置蓝牙
    int reset_result = system("hciconfig hci0 reset");
    if (reset_result != 0) {
        perror("Failed to reset Bluetooth adapter");
        return 1;
    }
    
    // 获取蓝牙适配器的设备 ID
    int dev_id = hci_get_route(NULL);
    if (dev_id < 0) {
        perror("No Bluetooth adapter found");
        return 1;
    }

    // 执行 LE 扫描
    return lescan(dev_id, argc, argv);
}