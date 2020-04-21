#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <keys.h> //Wifi and RemoteMe API
#include <AccelStepper.h>
#include <ArduinoOTA.h>


// Update these with values suitable for your network.

const char *mqtt_server = "192.168.1.183";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

#define STEPPIN 14
#define DIRPIN 4
#define EPIN 5
#define STEPS 575000

int flag = 0;
int boot = 1;
int inch = 0;

AccelStepper stepper(AccelStepper::DRIVER, STEPPIN, DIRPIN);

void setup_wifi()
{

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_NAME);
    WiFi.hostname("Smart_Blinds");

    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    payload[length] = '\0';
    String position = String((char*)payload);

    Serial.print(position);
    Serial.println();

    //stepper
    if (boot == 1) {
        if (position == "close") {
            stepper.setCurrentPosition(0);
            client.publish("home-assistant/desk_cover/response", "OFF");
        }
        if (position == "open") {
            stepper.setCurrentPosition(STEPS);
            client.publish("home-assistant/desk_cover/response", "ON");
        }
        if (position == "mid") {
            stepper.setCurrentPosition(STEPS/2);
            client.publish("home-assistant/desk_cover/response", "ON");
        }
        if (position == "inch") {
            stepper.moveTo(0);
        }
        boot = 0;
    } else {

        if (position == "close") {
            client.publish("home-assistant/desk_cover/response", "OFF");
            stepper.moveTo(0);
        }
        if (position == "open") {
            client.publish("home-assistant/desk_cover/response", "ON");
            stepper.moveTo(STEPS);
        }
        if (position == "mid") {
            client.publish("home-assistant/desk_cover/response", "ON");
            stepper.moveTo(STEPS/2);
        }
        if (position == "inch") {
            stepper.moveTo(5000);
            inch = 1;
        }

        stepper.enableOutputs();
        stepper.setSpeed(10000);

        flag = 0;
    }

}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        stepper.disableOutputs();
        // Create a random client ID
        String clientId = "Desk_Cover-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish("home-assistant/desk_cover/availability", "online");
            // ... and resubscribe
            client.subscribe("home-assistant/desk_cover/set");
            client.subscribe("home-assistant/desk_cover/set_position");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    ArduinoOTA.begin();
    stepper.setMaxSpeed(200000);
    stepper.setAcceleration(10000);
    stepper.setEnablePin(EPIN);
    stepper.disableOutputs();
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

}

void loop()
{
    ArduinoOTA.handle();

    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    unsigned long now = millis();
    if (now - lastMsg > 60000)
    {
        lastMsg = now;
        client.publish("home-assistant/desk_cover/availability", "online");
    }

    stepper.runSpeedToPosition();

    if(stepper.distanceToGo() == 0 && flag == 0) {
        stepper.disableOutputs();
        flag = 1;
        if (inch == 1)
        {
            stepper.setCurrentPosition(0);
            inch = 0;
        }
        
    }
}