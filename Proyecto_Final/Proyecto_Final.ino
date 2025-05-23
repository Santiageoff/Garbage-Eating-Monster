#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
#define SERVO_UP 9
#define SERVO_DOWN 10

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define FORWARD 1
#define STOP 0
#define BACKWARD -1

Servo motor1;
Servo motor2;
Servo servo;

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;
};

std::vector<MOTOR_PINS> motorPins =
{
  {18, 19},  // RIGHT_MOTOR
  {23,  5},  // LEFT_MOTOR
};

const char* ssid = "TU_NOMBRE_DE_RED";
const char* password = "TU_CONTRASEÑA";


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
    <style>
      .arrows {
        font-size:70px;
        color:white;
      }
      .circularArrows {
        font-size:80px;
        color:white;
      }
      td {
        background-color:#0039a6;
        border-radius:25%;
        box-shadow: 5px 5px #888888;
      }
      td:active {
        transform: translate(5px,5px);
        box-shadow: none; 
      }
      .noselect {
        -webkit-touch-callout: none; /* iOS Safari */
        -webkit-user-select: none; /* Safari */
        -khtml-user-select: none; /* Konqueror HTML */
        -moz-user-select: none; /* Firefox */
        -ms-user-select: none; /* Internet Explorer/Edge */
        user-select: none; /* Non-prefixed version, currently supported by Chrome and Opera */
      }
    </style>
  </head>
  <body class="noselect" align="center" style="background-color:white">
    <h1 style="color: black;text-align:center;">Garbage Eating Monster</h1>
    <h2 style="color: black;text-align:center;">Control Wi-Fi &#128663;</h2>
    <table id="mainTable" style="width:400px;margin:auto;table-layout:fixed" CELLSPACING=10>
      <tr>
        <td ontouchstart='onTouchStartAndEnd("5")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11017;</span></td>
        <td ontouchstart='onTouchStartAndEnd("1")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8679;</span></td>
        <td ontouchstart='onTouchStartAndEnd("6")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11016;</span></td>
      </tr>
      <tr>
        <td ontouchstart='onTouchStartAndEnd("3")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8678;</span></td>
        <td></td>    
        <td ontouchstart='onTouchStartAndEnd("4")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8680;</span></td>
      </tr>
      <tr>
        <td ontouchstart='onTouchStartAndEnd("7")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11019;</span></td>
        <td ontouchstart='onTouchStartAndEnd("2")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#8681;</span></td>
        <td ontouchstart='onTouchStartAndEnd("8")' ontouchend='onTouchStartAndEnd("0")'><span class="arrows" >&#11018;</span></td>
      </tr>
      <tr>
        <td ontouchstart='onTouchStartAndEnd("9")' ontouchend='onTouchStartAndEnd("0")'><span class="circularArrows" >&#8634;</span></td>
        <td style="background-color:white;box-shadow:none"></td>
        <td ontouchstart='onTouchStartAndEnd("10")' ontouchend='onTouchStartAndEnd("0")'><span class="circularArrows" >&#8635;</span></td>
      </tr>
    </table>
    <script>
      var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
      var websocket;
      
      function initWebSocket() 
      {
        websocket = new WebSocket(webSocketUrl);
        websocket.onopen    = function(event){};
        websocket.onclose   = function(event){setTimeout(initWebSocket, 2000);};
        websocket.onmessage = function(event){};
      }

      function onTouchStartAndEnd(value) 
      {
        websocket.send(value);
      }

      function moveServo(direction) 
      {
        if (direction == "UP") {
          websocket.send("100"); // Envía el comando para levantar el servo
        } else if (direction == "DOWN") {
          websocket.send("200"); // Envía el comando para bajar el servo
        }
      }
          
      window.onload = initWebSocket;
      document.getElementById("mainTable").addEventListener("touchend", function(event){
        event.preventDefault()
      });      
    </script>
  </body>
</html>
)HTMLHOMEPAGE";

void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);    
  }
  else if (motorDirection == BACKWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);     
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
}

void processCarMovement(String inputValue)
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());  
  switch(inputValue.toInt())
  {
    case UP:
      rotateMotor(RIGHT_MOTOR, 1);
      rotateMotor(LEFT_MOTOR, -1);                  
      break;
  
    case DOWN:
      rotateMotor(RIGHT_MOTOR, -1);
      rotateMotor(LEFT_MOTOR, 1);
      break;
  
    case LEFT:
      rotateMotor(RIGHT_MOTOR, 1);
      rotateMotor(LEFT_MOTOR, 1); 
      break;
  
    case RIGHT:
      rotateMotor(RIGHT_MOTOR, -1);
      rotateMotor(LEFT_MOTOR, -1);
      break;
  
    case UP_LEFT:
      rotateMotor(RIGHT_MOTOR, 1);
      rotateMotor(LEFT_MOTOR, 0);  
      break;
  
    case UP_RIGHT:
      rotateMotor(RIGHT_MOTOR, 0);
      rotateMotor(LEFT_MOTOR, -1);  
      break;
  
    case DOWN_LEFT:
      rotateMotor(RIGHT_MOTOR, -1);
      rotateMotor(LEFT_MOTOR, 0);  
      break;
  
    case DOWN_RIGHT:
      rotateMotor(RIGHT_MOTOR, 0);
      rotateMotor(LEFT_MOTOR, 1);   
      break;
  
    case STOP:
      rotateMotor(RIGHT_MOTOR, 0);
      rotateMotor(LEFT_MOTOR, 0);
      break;

    case SERVO_UP:
      motor1.write(180); // Mueve el servo 1 hacia arriba
      motor2.write(180); // Mueve el servo 2 hacia arriba
      break;

    case SERVO_DOWN:
      motor1.write(45); // Mueve el servo 1 hacia abajo
      motor2.write(45); // Mueve el servo 2 hacia abajo
      break;
  
    default:
      rotateMotor(RIGHT_MOTOR, 0);
      rotateMotor(LEFT_MOTOR, 0); 
      break;
  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
  request->send(404, "text/plain", "File Not Found");
}

void onWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        processCarMovement(myData.c_str());       
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}

void setUpPinModes()
{
  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    rotateMotor(i, STOP);  
  }
}

void setup(void) 
{
  setUpPinModes();
  Serial.begin(115200);

  motor1.attach(13); // Adjunta el servo 1 al pin 18
  motor2.attach(13); // Adjunta el servo 2 al pin 19

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", htmlHomePage);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() 
{
  ws.cleanupClients();
}