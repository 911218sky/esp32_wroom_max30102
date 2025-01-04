#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <WebServer.h>
#include <SPIFFS.h>

// Initialize and start web server
void setupServer();

// Handle incoming HTTP requests 
void handleServer();

// Print SPIFFS file listing
void listFiles();

// Get MIME type from file extension
String getContentType(String filename);

#endif