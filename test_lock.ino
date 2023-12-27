#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiClient.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <Servo.h>              // Include the Servo Library



ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

Servo myservo;              // Create servo object



void handleRoot();              // function prototypes for HTTP handlers
void handleLOCK();
void handleNotFound();

const char* ssid     = "SASKTEL05OT";
const char* password = "DJT(052901)";

boolean lock = false;


void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  
  if (MDNS.begin("lock")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

//  server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/ON", HTTP_POST, handleON);  // Call the 'handleON' function when a POST request is made to URI "/ON"
  server.on("/OFF", HTTP_POST, handleOFF);  // Call the 'handleOFF' function when a POST request is made to URI "/OFF"
  server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                          // Actually start the server
  Serial.println("HTTP server started");

  
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  
  pinMode(D4, OUTPUT);              // Set D4 as a output pin


  myservo.attach(0);            // Attach servo motor to GPIO 0 or D3 on ESP8266 board
  myservo.write(0);             // Set the initial position to 0
  
}

void loop() {
  MDNS.update();
  server.handleClient();                    // Listen for HTTP requests from clients
}

 void handleRoot() {
//  server.send(200, "text/plain", "Hello world!");   // Send HTTP status 200 (Ok) and send some text to the browser/client

    if(!lock){
//      server.send(200, "text/plain", "Locked! press the button to unlock");
      server.send(200, "text/html", "<form action=\"/ON\" method=\"POST\"><input type=\"submit\" value=\"On\"></form>");
      }
      else{
//        server.send(200, "text/plain", "Unlocked! press the button to lock");
        server.send(200, "text/html", "<form action=\"/OFF\" method=\"POST\"><input type=\"submit\" value=\"Off\"></form>");
      }
}

void handleON() {                          // If a POST request is made to URI /LOCK
  Serial.println("Button toggled ON!");          // to know if it is working properly
  myservo.write(180);                    // Turn the servo to 180 degree when toggled
  digitalWrite(D4,HIGH);      // Change the state of the LOCK
  lock = true;                    // Set the lock back to on
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303
}
void handleOFF(){
  Serial.println("Button toggled OFF");             // To test on Serial monitor
  myservo.write(0);                 // Turn the servo back to 0 degree
  digitalWrite(D4, LOW);                    // Turn off the onboard LED
  lock = false;                         // Set the lock to off
  server.sendHeader("Location","/");
  server.send(303);
  }

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
