#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <signal.h>
#include "bluetooth/bluetooth.h"
#include "bluetooth/hci.h"
#include "bluetooth/hci_lib.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Unofficial value, might still change */
#define LE_LINK 0x80

#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

#define EIR_NAME_SHORT 0x08  /* shortened local name */
#define EIR_NAME_COMPLETE 0x09  /* complete local name */

#define HCI_MAX_EVENT_SIZE 260

static volatile int signal_received = 0;

static void sigint_handler(int sig) 
{
    signal_received = sig; 
}

static void eir_parse_name(uint8_t *eir, size_t eir_len, char *buf, size_t buf_len)
{
    size_t offset = 0;

    while (offset < eir_len) {
        uint8_t field_len = eir[offset];
        size_t name_len;

        /* Check for the end of EIR */
        if (field_len == 0)
            break;

        if (offset + field_len > eir_len)
            goto failed;

        printf("EIR field type: 0x%02x, field length: %d\n", eir[offset + 1], field_len); // 添加调试信息

        // 打印EIR字段的内容
        printf("EIR field data: ");
        for (size_t i = 0; i < field_len; i++) {
            printf("%02x ", eir[offset + i]);
        }
        printf("\n");

        switch (eir[offset + 1]) {
            case EIR_NAME_SHORT:
            case EIR_NAME_COMPLETE:
                name_len = field_len - 1; 
                if (name_len > buf_len)
                    goto failed;

                memcpy(buf, &eir[offset + 2], name_len);
                buf[name_len] = '\0'; // 确保字符串以 null 结尾
                return;
            case 0x01: // Flags
                printf("Flags: 0x%02x\n", eir[offset + 2]);
                break;
            case 0x16: // Service Data
                printf("Service Data: ");
                for (size_t i = 2; i < field_len; i++) {
                    printf("%02x ", eir[offset + 2 + i]);
                }
                printf("\n");
                break;
            // 你可以在这里添加更多的case来解析其他类型的EIR字段
        }

        offset += field_len + 1; 
    }

failed:
    snprintf(buf, buf_len, "(unknown)");
}

static int print_advertising_devices(int dd)
{
    unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
    struct hci_filter nf, of;
    struct sigaction sa;
    socklen_t olen;
    int len;

    olen = sizeof(of);
    if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
        printf("Could not get socket options\n");
        return -1;
    }

    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);       // hci数据包类型，此处设置为事件数据包
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);   // hci事件数据包中具体事件类型。EVT_LE_META_EVENT为事件码。
    if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
        printf("Could not set socket options\n");
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_NOCLDSTOP;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    while (1) {
        evt_le_meta_event *meta;
        le_advertising_info *info;
        char addr[18];

        while ((len = read(dd, buf, sizeof(buf))) < 0) {
            if (errno == EINTR && signal_received == SIGINT) {
                len = 0;
                goto done;
            }
            if (errno == EAGAIN || errno == EINTR)
                continue;
            goto done;
        }

        ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
        len -= (1 + HCI_EVENT_HDR_SIZE);
        meta = (evt_le_meta_event *) ptr;
        if (meta->subevent != 0x02)
            goto done;

        /* Ignoring multiple reports */
        info = (le_advertising_info *) (meta->data + 1);
        {
            char name[248];
            memset(name, 0, sizeof(name));
            ba2str(&info->bdaddr, addr);
            printf("EIR Data Length: %d\n", info->length); // 添加调试信息
            eir_parse_name(info->data, info->length, name, sizeof(name) - 1);
            printf("%s %s\n", addr, name);
        }
    }

done:
    setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));
    if (len < 0)
        return -1;
    return 0;
}

int lescan(int dev_id, int argc, char **argv)
{
    int err, dd;
    uint8_t own_type = LE_PUBLIC_ADDRESS;
    uint8_t scan_type = 0x00;
    uint8_t filter_policy = 0x00;
    uint16_t interval = htobs(0x0010);
    uint16_t window = htobs(0x0010);
    uint8_t filter_dup = 0x01; //禁用重复过滤

    dd = hci_open_dev(dev_id);
    if (dd < 0) {
        perror("Could not open device");
        exit(1);
    }

    err = hci_le_set_scan_parameters(dd, scan_type, interval, window,
                                     own_type, filter_policy, 10000);
    if (err < 0) {
        perror("Set scan parameters failed");
        exit(1);
    }

    err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 10000);
    if (err < 0) {
        perror("Enable scan failed");
        exit(1);
    }

    printf("LE Scan ...\n");

    err = print_advertising_devices(dd);
    if (err < 0) {
        perror("Could not receive advertising events");
        exit(1);
    }

    err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 10000);
    if (err < 0) {
        perror("Disable scan failed");
        exit(1);
    }

    hci_close_dev(dd);
    return 0;
}

int main(int argc, char **argv)
{
    int dev_id = hci_get_route(NULL);
    if (dev_id < 0) {
        perror("No Bluetooth adapter found");
        return 1;
    }

    return lescan(dev_id, argc, argv);
}