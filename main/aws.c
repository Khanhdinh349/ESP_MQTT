#include "aws.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cJSON.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

TaskHandle_t AWSTaskHandle;
enum AWS_prov_state{
    CERTIFICATE_AND_KEY,
    PROVISIONING,
    THING_ACTIVE
};
enum AWS_prov_state aws_prov_state = CERTIFICATE_AND_KEY;

char MQTT_SUB_TOPIC[35];
char MQTT_PUB_TOPIC[35];
char CLIENT_ID[21];

static const char * TAG = "AWS";

static void AWSHandlerConnected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    int msg_id = 0;
    ESP_LOGI(TAG, "Subscribe to MQTT_SUB_TOPIC");
    msg_id = esp_mqtt_client_subscribe(client, MQTT_SUB_TOPIC, 1);
    ESP_LOGI(TAG, "Subscribe to MQTT_SUB_TOPIC successful, msg_id=%d", msg_id);
}

static void AWSHandlerData(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
}
void AWSTask(void *arg)
{   
    sprintf(MQTT_SUB_TOPIC,"%s%s%s%s%s", "ESP32", "/sub/", "ESP32", "_", "KHANH");
    printf("MQTT_SUB_TOPIC=%s\n",MQTT_SUB_TOPIC);
    printf("MQTT_SUB_TOPIC length =%d\n",strlen(MQTT_SUB_TOPIC));

    sprintf(MQTT_PUB_TOPIC,"%s%s%s%s%s", "ESP32", "/pub/", "ESP32", "_", "KHANH");
    printf("MQTT_PUB_TOPIC=%s\n",MQTT_PUB_TOPIC);
    printf("MQTT_PUB_TOPIC length =%d\n",strlen(MQTT_PUB_TOPIC));

    sprintf(CLIENT_ID,"%s%s%s", "ESP32", "_", "KHANH");
    printf("CLIENT_ID=%s\n",CLIENT_ID);
    printf("CLIENT_ID length =%d\n",strlen(CLIENT_ID));

    esp_mqtt_client_handle_t client;
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://a3suuuxay09k3c-ats.iot.us-west-2.amazonaws.com",
        .client_cert_pem = (const char *)client_crt_start,
        .client_key_pem = (const char *)client_key_start,
        .cert_pem = (const char *)root_pem_start,
        .client_id = CLIENT_ID,
        .buffer_size = 2048,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, AWSHandlerConnected, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, AWSHandlerData, NULL);
    esp_mqtt_client_start(client);
    
    while(1){
        ESP_LOGI(TAG,"========================RAM left %d==============================", esp_get_free_heap_size());
        int msg_id=0;
        char *data_json ="Test data";
        ESP_LOGI(TAG, "Publish to MQTT_PUB_TOPIC");
        msg_id = esp_mqtt_client_publish(client, MQTT_PUB_TOPIC, data_json, 0, 1, 0);
        ESP_LOGI(TAG, "sent publish MQTT_PUB_TOPIC successful, msg_id=%d", msg_id);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
/*-----------------------------------------------------------*/
void AWSRun(void){
    xTaskCreate(&AWSTask, "AWSTask", 3*1024, NULL, 5, &AWSTaskHandle);
}
/*-----------------------------------------------------------*/
void AWSStop(void){
    vTaskDelete(AWSTaskHandle);
}
/*-----------------------------------------------------------*/