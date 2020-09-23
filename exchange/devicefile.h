#ifndef DEVICEFILE_H
#define DEVICEFILE_H

#include <QMutex>


class DeviceFile
{
public:
    static DeviceFile* instance(){
//        if(! _instance) {
//            _instance = new DeviceFile();
//        }
        static DeviceFile p;
        return &p;
    };
    /**
     * @brief init Create lock-file, open device-file.
     * If lock-file exist return -1; if device-file dos not pened - return -2;
     * @return ERRORCODE
     */
    int init(const char* device);

    bool isLocked(){
        return _isLocked;
    }

    void unlock();

    void close_device();

    int open_device(char* devname);


    QMutex mutex;

private:
//    static DeviceFile* _instance;
    bool _isLocked;
    int fd;

    char* lockfile;

    char device_name[512] = {};

    DeviceFile(){
        _isLocked = false;
        fd = 0;
        lockfile = nullptr;
    }
};

#endif // DEVICEFILE_H
