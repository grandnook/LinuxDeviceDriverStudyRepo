#ifndef MY_DEVICE_DRIVER_H_
#define MY_DEVICE_DRIVER_H_

#include <linux/ioctl.h>

// ioctl用のパラメータ(第3引数)の定義
struct mydevice_values {
    int val1;
    int val2;
};


// ioctl用のコマンド(request, 第2引数)の定義
// このデバドラで使用するIOCTL用コマンドのタイプ，なんでもいいが，'M'にしてみる
#define MYDEVICE_IOC_TYPE 'M'

// デバドラに値を設定するコマンド
#define MYDEVICE_SET_VALUES _IOW(MYDEVICE_IOC_TYPE, 1, struct mydevice_values)

// デバドラから値を取得するコマンド
#define MYDEVICE_GET_VALUES _IOR(MYDEVICE_IOC_TYPE, 2, struct mydevice_values)


#endif  /* MY_DEVICE_DRIVER_H_ */
