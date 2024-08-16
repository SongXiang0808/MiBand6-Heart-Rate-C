# 扫描蓝牙设备包括低功耗蓝牙设备
# from bluepy.btle import Scanner, DefaultDelegate
# import time

# class ScanDelegate(DefaultDelegate):
#     def __init__(self):
#         DefaultDelegate.__init__(self)

#     def handleDiscovery(self, dev, isNewDev, isNewData):
#         if isNewDev:
#             print("Discovered device", dev.addr)
#         elif isNewData:
#             print("Received new data from", dev.addr)

# scanner = Scanner().withDelegate(ScanDelegate())

# try:
#     print("Scanning... Press Ctrl+C to stop.")
#     while True:
#         devices = scanner.scan(1.0)  # Scan for 1 second at a time
#         for device in devices:
#             print(f"Device {device.addr}")
#             print(f"  RSSI: {device.rssi}")
#             for (adtype, desc, value) in device.getScanData():
#                 print(f"  {desc}: {value}")
#         time.sleep(1.0)  # Sleep to avoid excessive scanning
# except KeyboardInterrupt:
#     print("Stopped scanning.")


# 不带时间戳的接收小米手环心率数据
# from bluepy.btle import Scanner, DefaultDelegate
# import time

# class ScanDelegate(DefaultDelegate):
#     def __init__(self):
#         DefaultDelegate.__init__(self)

#     def handleDiscovery(self, dev, isNewDev, isNewData):
#         if isNewDev or isNewData:
#             self.process_device(dev)

#     def process_device(self, dev):
#         adv_data = dev.getScanData()
#         manufacturer_data = None
#         for (adtype, desc, value) in adv_data:
#             if adtype == 0xFF:  # Manufacturer Specific Data
#                 manufacturer_data = value
#                 break

#         if manufacturer_data and len(manufacturer_data) >= 8:
#             company_id = int(manufacturer_data[0:4], 16)
#             if company_id == 0x5701:
#                 name = dev.addr
#                 rssi = dev.rssi
#                 heart_rate_hex = manufacturer_data[10:12]
#                 heart_rate = int(heart_rate_hex, 16)
#                 print(f"{name} ({rssi}dBm) Heart Rate: {heart_rate}")

# def main():
#     scanner = Scanner().withDelegate(ScanDelegate())
#     print("Starting scan")
#     while True:
#         devices = scanner.scan(2.0)  # Scan for 10 seconds at a time
#         time.sleep(1.0)  # Sleep to avoid excessive scanning

# if __name__ == "__main__":
#     try:
#         main()
#     except KeyboardInterrupt:
#         print("Stopped scanning.")


# 带时间戳的接收小米手环心率数据
from bluepy.btle import Scanner, DefaultDelegate
import time
from datetime import datetime

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev or isNewData:
            self.process_device(dev)

    def process_device(self, dev):
        adv_data = dev.getScanData()
        manufacturer_data = None
        for (adtype, desc, value) in adv_data:
            if adtype == 0xFF:  # Manufacturer Specific Data
                manufacturer_data = value
                break

        if manufacturer_data and len(manufacturer_data) >= 14:  # Ensure data is long enough
            company_id = int(manufacturer_data[0:4], 16)
            if company_id == 0x5701:  # Corrected company ID
                name = dev.addr
                rssi = dev.rssi
                # Correct position for heart rate
                heart_rate_hex = manufacturer_data[10:12]  # Adjusted index
                heart_rate = int(heart_rate_hex, 16)
                timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                print(f"{timestamp} - {name} ({rssi}dBm) Heart Rate: {heart_rate}")

def main():
    scanner = Scanner().withDelegate(ScanDelegate())
    print("Starting scan")
    while True:
        devices = scanner.scan(2.0)  # Scan for 2 seconds at a time
        time.sleep(1.0)  # Sleep to avoid excessive scanning

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("Stopped scanning.")
