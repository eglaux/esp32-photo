#include "camera.h"

#include <Arduino.h>

#include "esp_camera.h"
#include "SD_MMC.h"


/*
*  Definició dels pins ESP32-CAM OV2640
*/
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1

#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22



// Comptador de fotos per afegir al nom del fitxer de cada imatge nova
static uint32_t photoCounter = 0;


// Llegir el valor del comptador de fotos de la SD si existeix i establir aquest valor a photoCounter
static void load_counter()
{

    File f =
        SD_MMC.open(
            "/photos/counter.txt"
        );


    if(!f)
    {
        photoCounter = 0;
        return;
    }


    photoCounter =
        f.readString().toInt();


    f.close();
}


// Actualitzar el comptador actual
static void save_counter()
{
    File f =
        SD_MMC.open(
            "/photos/counter.txt",
            FILE_WRITE
        );


    if(f)
    {
        f.print(
            photoCounter
        );

        f.close();
    }
}


// Inicialització càmara
void camera_init()
{
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;

    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;

    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;

    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;

    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;

    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound())
    {
        // Màxima resolució soportada per OV2640
        config.frame_size = FRAMESIZE_UXGA;

        // Qualitat JPEG, contra més baixa millor qualitat (0-63)
        config.jpeg_quality = 6;

        // Dos frame buffer si hi ha PSRAM a l'ESP32
        config.fb_count = 2;

        config.grab_mode = CAMERA_GRAB_LATEST;
    }
    else
    {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 10;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK)
    {
        // Error d'inicialització
        Serial.printf("Camera init failed: 0x%x\n", err);
        return;
    }

    // Ajustar sensor
    sensor_t *s = esp_camera_sensor_get();

    s->set_quality(s, 6);
    s->set_framesize(s, FRAMESIZE_UXGA);

    s->set_brightness(s, 0);
    s->set_contrast(s, 1);
    s->set_saturation(s, 0);
    s->set_sharpness(s, 2);

    s->set_denoise(s, 0);

    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);

    s->set_gain_ctrl(s, 1);
    s->set_exposure_ctrl(s, 1);

    s->set_hmirror(s, 0);
    s->set_vflip(s, 0);


    load_counter();


    Serial.println(
        "Camera ready"
    );
}



// Capturar imatge i desar-la
String capture_photo()
{
    // Activar flash
    analogWrite(GPIO_NUM_4, 30);
    delay(500);

    for (int i = 0; i < 8; i++)
    {
        camera_fb_t *fb = esp_camera_fb_get();

        if (fb)
            esp_camera_fb_return(fb);

        delay(150);
    }

    camera_fb_t *fb = esp_camera_fb_get();

    //desactivar flash
    analogWrite(GPIO_NUM_4, 0);

    if(!fb)
    {
        Serial.println(
            "Capture failed"
        );

        return "error";
    }

    photoCounter++;
    char filename[64];
    snprintf(
        filename,
        sizeof(filename),
        "/photos/photo_%06lu.jpg",
        (unsigned long)photoCounter
    );

    // obrir fitxer per escriure-hi imatge
    File file =
        SD_MMC.open(
            filename,
            FILE_WRITE
        );



    if(!file)
    {
        esp_camera_fb_return(
            fb
        );

        return "";
    }

    // Escriure imatge a fitxer
    file.write(
        fb->buf,
        fb->len
    );

    file.close();


    esp_camera_fb_return(
        fb
    );

    // Actualitzar comptador
    save_counter();

    
    Serial.print(
        "Saved: "
    );
    Serial.println(
        filename
    );

    return String(filename);
}