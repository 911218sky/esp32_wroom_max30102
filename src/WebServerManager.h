#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WebServer.h>
#include <SPIFFS.h>

void setupServer();
void handleServer();
void listFiles();
String getContentType(String filename);

#endif
