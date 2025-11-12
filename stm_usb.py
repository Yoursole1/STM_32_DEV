import usb.core
found = False
for dev in usb.core.find(find_all=True): # type: ignore
    if dev.manufacturer == "STMicroelectronics": # pyright: ignore[reportAttributeAccessIssue]
        dev.reset() # type: ignore
        found = True
        break
if found:
    print("STM found and reset")
else:
    print("STM not found")