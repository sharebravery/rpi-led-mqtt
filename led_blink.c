#include <stdio.h>
#include <unistd.h> // For usleep function
#include <wiringPi.h>

#define LED_PIN 21

int main(void)
{
    // 初始化wiringPi
    if (wiringPiSetupGpio() == -1) {
        printf("setup wiringPi failed!");
        return 1;
    }

    // 设置GPIO21为输出模式
    pinMode(LED_PIN, OUTPUT);

    while (1) {
        // 打开LED
        digitalWrite(LED_PIN, HIGH);
        usleep(500000); // 延迟500毫秒

        // 关闭LED
        digitalWrite(LED_PIN, LOW);
        usleep(500000); // 延迟500毫秒
    }

    return 0;
}

