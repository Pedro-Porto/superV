#include "keyboard_emulator.hpp"

/**
 * @brief Sends a key event to the uinput device
 */
void sendKey(int fd, int keycode, bool press) {
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = press ? 1 : 0;
    write(fd, &ev, sizeof(ev));

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(fd, &ev, sizeof(ev));
}

/**
 * @brief Waits for the device to be created
 */
bool wait_for_device(const char* device_path, int timeout_ms) {
    struct stat buffer;
    int elapsed = 0;

    while (elapsed < timeout_ms) {
        if (stat(device_path, &buffer) == 0) {
            return true;
        }
        usleep(10000);
        elapsed += 10;
    }
    return false;
}

/**
 * @brief Simulates a Ctrl + V key press
 * 
 * This function simulates a Ctrl + V key press by creating a uinput device and sending the key events.
 * A keyboard is created at kernel level and the key events are sent to the device.
 */
void ctrlV() { // kernel level keyboard simulation
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "error opening /dev/uinput" << std::endl;
        return;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTCTRL);
    ioctl(fd, UI_SET_KEYBIT, KEY_V);

    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    strcpy(usetup.name, "superV keyboard");

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);

    if (!wait_for_device("/dev/uinput", 500)) {
        std::cerr << "uinput device creation failure" << std::endl;
        close(fd);
        return;
    }
    usleep(100000);

    sendKey(fd, KEY_LEFTCTRL, true);
    sendKey(fd, KEY_V, true);
    sendKey(fd, KEY_V, false);
    sendKey(fd, KEY_LEFTCTRL, false);

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

    std::cout << "Ctrl + V" << std::endl;
}