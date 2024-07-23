cmake_minimum_required(VERSION 3.17)
project(mqtt_c C)

set(CMAKE_C_STANDARD 99)

# 手动设置包含目录和库目录
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# 添加可执行文件
add_executable(mqtt_c main.c)

# 链接Paho MQTT C库和wiringPi库
target_link_libraries(mqtt_c paho-mqtt3c wiringPi)
