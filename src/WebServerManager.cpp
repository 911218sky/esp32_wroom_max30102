#include "WebServerManager.h"

WebServer server(80);
IPAddress localIP(192, 168, 4, 1);

void listFiles() {
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }

    Serial.println("Files in SPIFFS:");
    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            Serial.print("  File: ");
            Serial.print(file.name());
            Serial.print("\tSize: ");
            Serial.println(file.size());
        } else {
            Serial.print("  Directory: ");
            Serial.println(file.name());
        }
        file = root.openNextFile();
    }
}

String getContentType(String filename) {
    if (filename.endsWith(".html") || filename.endsWith(".html.gz"))
        return "text/html";
    if (filename.endsWith(".css") || filename.endsWith(".css.gz"))
        return "text/css";
    if (filename.endsWith(".js") || filename.endsWith(".js.gz"))
        return "application/javascript";
    if (filename.endsWith(".json") || filename.endsWith(".json.gz"))
        return "application/json";
    if (filename.endsWith(".png") || filename.endsWith(".png.gz"))
        return "image/png";
    if (filename.endsWith(".jpg") || filename.endsWith(".jpg.gz"))
        return "image/jpeg";
    if (filename.endsWith(".gif") || filename.endsWith(".gif.gz"))
        return "image/gif";
    if (filename.endsWith(".svg") || filename.endsWith(".svg.gz"))
        return "image/svg+xml";
    if (filename.endsWith(".ico") || filename.endsWith(".ico.gz"))
        return "image/x-icon";
    return "text/plain";
}

void setupServer() {
    WiFi.softAPConfig(localIP, localIP, IPAddress(255, 255, 255, 0));

    server.on("/", HTTP_GET, []() {
        String path = "/index.html";
        if (SPIFFS.exists(path + ".gz")) {
            path += ".gz";
        }
        if (SPIFFS.exists(path)) {
            File file = SPIFFS.open(path, "r");
            server.streamFile(file, getContentType(path));
            file.close();
        } else {
            server.send(404, "text/plain", "File Not Found");
        }
    });

    server.onNotFound([]() {
        String path = server.uri();
        if (SPIFFS.exists(path + ".gz")) {
            path += ".gz";
        }

        if (SPIFFS.exists(path)) {
            File file = SPIFFS.open(path, "r");
            server.streamFile(file, getContentType(path));
            file.close();
        } else {
            server.send(404, "text/plain", "File Not Found: " + server.uri());
        }
    });

    server.begin();
    Serial.println("HTTP server started");
}

void handleServer() {
    server.handleClient();
}
