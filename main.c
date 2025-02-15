#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <MQTTClient.h>

// Enable or disable SSL/TLS connection (1 for SSL/TLS, 0 for TCP)
#define USE_SSL 0

#if USE_SSL
#define ADDRESS "ssl://broker.emqx.io:8883"
#else
#define ADDRESS "tcp://broker.emqx.io:1883"
#endif

#define USERNAME "emqx"
#define PASSWORD "public"
#define CLIENTID "c-client"
#define QOS 0
#define TOPIC "emqx/c-test9527"
#define TIMEOUT 10000L
#define LED_PIN 21

int blink = 0;

MQTTClient_SSLOptions configureSSLOptions()
{
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
    ssl_opts.enableServerCertAuth = 1;
    ssl_opts.trustStore = "/home/pi/CA/emqxsl-ca.crt";
    return ssl_opts;
}

void controlLED(const char *payload)
{
    if (strcmp(payload, "ON") == 0)
    {
        blink = 0;
        digitalWrite(LED_PIN, LOW); // 反转 LED 控制
        printf("LED turned ON\n");
    }
    else if (strcmp(payload, "OFF") == 0)
    {
        blink = 0;
        digitalWrite(LED_PIN, HIGH); // 反转 LED 控制
        printf("LED turned OFF\n");
    }
    else if (strcmp(payload, "BLINK") == 0)
    {
        blink = 1;
        printf("LED set to BLINK mode\n");
    }
    else
    {
        printf("Unknown command: %s\n", payload);
    }
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char *payload = (char *)message->payload;
    printf("Received `%s` from `%s` topic\n", payload, topicName);
    controlLED(payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main(int argc, char *argv[])
{
    int rc;
    MQTTClient client;

    // 初始化 WiringPi，使用 BCM GPIO 编号
    if (wiringPiSetupGpio() == -1)
    {
        printf("WiringPi setup failed\n");
        exit(1);
    }
    pinMode(LED_PIN, OUTPUT);

    // 创建 MQTT 客户端
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

#if USE_SSL
    MQTTClient_SSLOptions ssl_opts = configureSSLOptions();
    conn_opts.ssl = &ssl_opts;
#endif

    // 设置回调函数
    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    // 连接到 MQTT 服务器
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    else
    {
        printf("Connected to MQTT Broker!\n");
    }

    // 订阅主题
    MQTTClient_subscribe(client, TOPIC, QOS);

    // 无限循环保持程序运行
    while (1)
    {
        if (blink)
        {
            digitalWrite(LED_PIN, LOW);
            usleep(500000); // 延迟500毫秒
            digitalWrite(LED_PIN, HIGH);
            usleep(500000); // 延迟500毫秒
        }
        else
        {
            sleep(1);
        }
    }

    // 清理和退出
    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, TIMEOUT);
    MQTTClient_destroy(&client);
    return rc;
}
