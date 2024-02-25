
// To use `bcm_host_get_peripheral_address()`
//     gcc userGpio.c -L/opt/vc/lib -lbcm_host

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

// ペリフェラルレジスタの物理アドレス(BCM2835の仕様書より)
// https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf
#define REG_ADDR_BASE               (0x3F000000)  // bcm_host_get_peripheral_address()の方がbetter
#define REG_ADDR_GPIO_BASE          (REG_ADDR_BASE + 0x00200000)
#define REG_ADDR_GPIO_LENGTH        (4096)    // ページサイズの4KB
#define REG_ADDR_GPIO_GPFSEL_0      (0x0000)  // 出力モードに設定する
#define REG_ADDR_GPIO_OUTPUT_SET_0  (0x001C)  // High出力する
#define REG_ADDR_GPIO_OUTPUT_CLR_0  (0x0028)  // Low出力する
#define REG_ADDR_GPIO_LEVEL_0       (0x0034)  // 現在の出力レベルを取得する

#define REG(addr) (*((volatile unsigned int *)(addr)))
#define DUMP_REG(addr) printf("%08X\n", REG(addr));

int main()
{
    void *address;  // アドレス（メモリの番地なので<void *>型で宣言する）
    int fd;

    // メモリアクセス用のデバイスファイルを開く
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        perror("open");
        return -1;
    }

    // ARM(CPU)から見た物理アドレス → 仮想アドレスへのマッピング
    address = mmap(NULL, REG_ADDR_GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, REG_ADDR_GPIO_BASE);
    if (address == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }
    printf("physical address = %p\n", REG_ADDR_GPIO_BASE);
    printf("virtual  address = %p\n", address);

    // GPIO4を出力に設定
    DUMP_REG(address + REG_ADDR_GPIO_GPFSEL_0);
    REG(address + REG_ADDR_GPIO_GPFSEL_0) = 1 << 12;
    DUMP_REG(address + REG_ADDR_GPIO_GPFSEL_0);

    // GPIO4をHigh出力
    DUMP_REG(address + REG_ADDR_GPIO_OUTPUT_SET_0);
    REG(address + REG_ADDR_GPIO_OUTPUT_SET_0) = 1 << 4;
    DUMP_REG(address + REG_ADDR_GPIO_OUTPUT_SET_0);

    // GPIO4をLow出力
    DUMP_REG(address + REG_ADDR_GPIO_OUTPUT_CLR_0);
    REG(address + REG_ADDR_GPIO_OUTPUT_CLR_0) = 1 << 4;
    DUMP_REG(address + REG_ADDR_GPIO_OUTPUT_CLR_0);

    munmap((void *)address, REG_ADDR_GPIO_LENGTH);
    close(fd);

    return 0;
}

