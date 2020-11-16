#include <stdio.h>
#include <string.h>

#include "lptimer.h"
#include "thread.h"

#include "periph/pm.h"
#include "pm_layered.h"

#include "cayenne_lpp.h"

#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */

typedef enum {
    APP_MSG_SEND,
} app_message_types_t;

typedef struct {
    uint8_t *buffer;
    uint32_t length;
} lora_data_t;

static sx127x_params_t sx127x_params = {
    .nss_pin = SX127X_SPI_NSS,
    .spi = SX127X_SPI,

    .dio0_pin = SX127X_DIO0,
    .dio1_pin = SX127X_DIO1,
    .dio2_pin = SX127X_DIO2,
    .dio3_pin = SX127X_DIO3,
    .dio4_pin = SX127X_DIO4,
    .dio5_pin = SX127X_DIO5,
    .reset_pin = SX127X_RESET,
   
    .rfswitch_pin = SX127X_RFSWITCH,
};

static semtech_loramac_t loramac;  /* The loramac stack device descriptor */
static kernel_pid_t sender_pid;  /* Pid of a thread which controls data sending */
static kernel_pid_t loramac_pid;  /* Pid of a thread which controls data sending */

/* define the required keys for activation */
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x80, 0x7b, 0x85, 0x90, 0x20, 0x00, 0x06, 0x23 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* we need AppKey for OTAA */
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x9f, 0xf4, 0x0a, 0x59, 0xa6, 0x03, 0x80, 0xd3, 0x20, 0x14, 0xfd, 0x35, 0x72, 0x40, 0x98, 0x98 };

static char bmp280_stack[THREAD_STACKSIZE_DEFAULT];
static cayenne_lpp_t bmp_lpp;

void *bmp280_thread(void *arg) {
    (void) arg;
    static msg_t data_msg = { .type = APP_MSG_SEND };
    static lora_data_t data;
    
    lptimer_ticks32_t last_wakeup = lptimer_now();
    
    while(1){
        /* Put data into Cayenne LPP buffer */
        cayenne_lpp_reset(&bmp_lpp);
        cayenne_lpp_add_temperature(&bmp_lpp, 1, 255); /* degrees C * 10 */

        /* Send a message to a LoRaWAN thread */
        data.buffer = bmp_lpp.buffer;
        data.length = bmp_lpp.cursor;
        data_msg.content.ptr = &data;
        msg_send(&data_msg, sender_pid);
        
        lptimer_periodic_wakeup(&last_wakeup, 10000);
    }
    
    return NULL;
}

#define MAX_RETR 10

int main(void){
    int res;
    
    /* Adjust message queue size */
    msg_t msg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);
    
    sx127x_params.rfswitch_active_level = SX127X_GET_RFSWITCH_ACTIVE_LEVEL(); /* Should be 1 on our prototype boards */
    
    loramac_pid = semtech_loramac_init(&loramac, &sx127x_params);
    sender_pid = thread_getpid();
    
    /* Set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
        
    semtech_loramac_set_tx_power(&loramac, LORAMAC_TX_PWR_0);
    semtech_loramac_set_dr(&loramac, 0);
    semtech_loramac_set_retries(&loramac, 0); /* Process failed TX attemts in the app */
    semtech_loramac_set_tx_mode(&loramac, LORAMAC_TX_CNF);   /* confirmed packets */
    semtech_loramac_set_tx_port(&loramac, LORAMAC_DEFAULT_TX_PORT); /* port 2 */
    //semtech_loramac_set_class(&loramac, LORAMAC_CLASS_C); /* Always listen */
    
    puts("[LoRa] LoRaMAC initialised");
    
    pm_unblock(PM_SLEEP);
    pm_unblock(PM_IDLE);
    
    do{
        res = semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA);
                    
        switch (res) {
        case SEMTECH_LORAMAC_JOIN_SUCCEEDED: 
            puts("[LoRa] successfully joined to the network");
            break;
        
        case SEMTECH_LORAMAC_ALREADY_JOINED:
            /* ignore, can't be */
            break;
        
        case SEMTECH_LORAMAC_RESTRICTED:
        case SEMTECH_LORAMAC_BUSY:
        case SEMTECH_LORAMAC_NOT_JOINED:
        case SEMTECH_LORAMAC_JOIN_FAILED:
        case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
            printf("[LoRa] LoRaMAC join failed: code %d\n", res);
            lptimer_sleep(10000);
            break;
        
        default:
            printf("[LoRa] join request: unknown response %d\n", res);
            break;
        }
    } while (res != SEMTECH_LORAMAC_JOIN_SUCCEEDED);
    
    /* Start another thread for sensor measurements */
    thread_create(bmp280_stack, sizeof(bmp280_stack),
              THREAD_PRIORITY_MAIN-1,
              THREAD_CREATE_STACKTEST,
              bmp280_thread,
              NULL,
              "Luminoсity");
    
    while (1) {
        msg_receive(&msg);

        if (msg.sender_pid != loramac_pid) {
            /* Application message */
            if (msg.type == APP_MSG_SEND) {
                lora_data_t *data = msg.content.ptr;
                (void) data;
                res = semtech_loramac_send(&loramac, data->buffer, data->length);

                switch (res) {
                case SEMTECH_LORAMAC_BUSY:
                    puts("[LoRa] MAC already busy");
                    break;
                case SEMTECH_LORAMAC_NOT_JOINED: 
                    puts("[LoRa] Not joined to the network");
                    break;
                case SEMTECH_LORAMAC_TX_OK:
                    puts("[LoRa] TX is in progress");
                    break;
                case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                    puts("[LoRa] TX duty cycle restricted");
                    break;
                default:
                    printf("[LoRa] Unknown send() response %d\n", res);
                    break;
                }
            }
        } else {
            switch (msg.type) {
                case MSG_TYPE_LORAMAC_TX_STATUS: 
                    if (msg.content.value == SEMTECH_LORAMAC_TX_DONE) {
                        puts("[LoRa] TX done");
                        break;
                    }
                    
                    if (msg.content.value == SEMTECH_LORAMAC_TX_CNF_FAILED) {
                        puts("[LoRa] Uplink confirmation failed");
                        /* TODO: rejoin if there are too many failures */
                        break;
                    }
                    
                    printf("[LoRa] Unknown TX status %lu\n", msg.content.value);
                    break;
                
                case MSG_TYPE_LORAMAC_RX: 
                    if ((loramac.rx_data.payload_len == 0) && loramac.rx_data.ack) {
                        printf("[LoRa] Ack received: RSSI %d, DR %d\n",
                                loramac.rx_data.rssi,
                                loramac.rx_data.datarate);
                    } else {
                        printf("[LoRa] Data received: %d bytes, port %d, RSSI %d, DR %d\n",
                                loramac.rx_data.payload_len,
                                loramac.rx_data.port,
                                loramac.rx_data.rssi,
                                loramac.rx_data.datarate);

                        printf("[LoRa] Hex data: ");
                        for (int l = 0; l < loramac.rx_data.payload_len; l++) {
                            printf("%02X ", loramac.rx_data.payload[l]);
                        }
                        printf("\n");

                        /* TODO: process data here */
                    }
                    break;
                
                case MSG_TYPE_LORAMAC_JOIN:
                    puts("[LoRa] LoRaMAC join notification\n");
                    break;
                    
                default:
                    printf("[LoRa] Unidentified LoRaMAC msg type %d\n", msg.type);
                    break;
            }
        }
    }
}
