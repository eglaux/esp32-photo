#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SD_MMC.h>
#include "camera.h"

#define WIFI_PASSWORD "esppass1"


WebServer server(80);

// ### Frontal web ###
String getContentType(const String &path)
{
    if (path.endsWith(".html")) return "text/html";
    if (path.endsWith(".css")) return "text/css";
    if (path.endsWith(".js")) return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".png")) return "image/png";
    if (path.endsWith(".jpg")) return "image/jpeg";
    if (path.endsWith(".svg")) return "image/svg+xml";
    return "text/plain";
}

bool serveFile(String path)
{
    if (!SD_MMC.exists(path))
        return false;

    File file = SD_MMC.open(path);

    server.streamFile(file, getContentType(path));

    file.close();

    return true;
}
// ### final Frontal web ###


// ### API handlers ###

/**
 * Captura una foto
 */
void capture()
{
    String f =
        capture_photo();


    server.send(
        200,
        "text/plain",
        f
    );
}


/**
 * llista totes les imatges de la carpeta
 */
void listFiles()
{

    File dir =
        SD_MMC.open(
            "/photos"
        );


    String out="[";


    File f =
        dir.openNextFile();


    bool first=true;


    while(f)
    {

        if(!first)
            out+=",";


        out+="\"";
        out+=f.name();
        out+="\"";


        first=false;


        f =
            dir.openNextFile();

    }

    out+="]";


    server.send(
        200,
        "application/json",
        out
    );
}


/*
 * Descarrega la imatge indicada per l'argument file
 */

void download()
{

    String name =
        "/photos/" + server.arg("file");


    File f =
        SD_MMC.open(name);



    if(!f)
    {
        server.send(
            404,
            "text/plain",
            "missing"
        );

        return;
    }


    server.streamFile(
        f,
        "image/jpeg"
    );


    f.close();

}
// ### end API handlers ###


void setup()
{
    Serial.begin(115200);
 
    // Pin del flash configurat com a output
    pinMode(GPIO_NUM_4, OUTPUT);

    WiFi.softAP(
        "ESP32-CAM",
        WIFI_PASSWORD
    );

    SD_MMC.begin(
        "/sdcard",
        true
    );

    // Comprovar existencia directori photos, si no, el crea i inicialitza
    if(!SD_MMC.exists("/photos")){
        SD_MMC.mkdir("/photos");

        File f =
        SD_MMC.open(
            "/photos/counter.txt",
            FILE_WRITE
        );


        if(f)
        {
            f.print(
                0
            );

            f.close();
        }
    }

    camera_init();

    // Fitxers del frontal web
    server.on("/", HTTP_GET, []()
    {
        serveFile("/web/index.html");
    });

    server.on("/style.css", HTTP_GET, []()
    {
        serveFile("/web/style.css");
    });

    server.on("/app.js", HTTP_GET, []()
    {
        serveFile("/web/app.js");
    });

    // Registrar els API Endpoints
    server.on(
        "/capture",
        capture
    );

    server.on(
        "/list",
        listFiles
    );

    server.on(
        "/download",
        download
    );

    server.begin();
}


void loop()
{
    server.handleClient();
}