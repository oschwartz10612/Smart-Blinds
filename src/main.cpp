#include <keys.h> //Wifi and RemoteMe API

#define STEPPIN 14
#define DIRPIN 4
#define EPIN 5
#define STEPS 575000

int flag = 0;
int boot = 1;

#include <RemoteMe.h>
#include <RemoteMeSocketConnector.h>
#include <ESP8266WiFi.h>
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, STEPPIN, DIRPIN);

RemoteMe& remoteMe = RemoteMe::getInstance(TOKEN, DEVICE_ID);

//*************** CODE FOR CONFORTABLE VARIABLE SET *********************

inline void setPosition(int32_t i) {remoteMe.getVariables()->setInteger("position", i); }

//*************** IMPLEMENT FUNCTIONS BELOW *********************

void onPositionChange(int32_t position) {

    if (boot == 1) {
        if (position == 0) {
            stepper.setCurrentPosition(0);
        }
        if (position == 1) {
            stepper.setCurrentPosition(STEPS);
        }
        if (position == 2) {
            stepper.setCurrentPosition(STEPS/2);
        }
        boot = 0;
    } else {

        if (position == 0) {
            stepper.moveTo(0);
        }
        if (position == 1) {
            stepper.moveTo(STEPS);
        }
        if (position == 2) {
            stepper.moveTo(STEPS/2);
        }

        stepper.enableOutputs();
        stepper.setSpeed(10000);

        flag = 0;
    }
}

void setup() {
    stepper.setMaxSpeed(200000); 
    stepper.setAcceleration(10000);
    stepper.setEnablePin(EPIN);

    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }

    remoteMe.getVariables()->observeInteger("position", onPositionChange);
    remoteMe.setConnector(new RemoteMeSocketConnector());
    remoteMe.sendRegisterDeviceMessage(DEVICE_NAME);
}


void loop() {
    remoteMe.loop();

    stepper.runSpeedToPosition();

    if(stepper.distanceToGo() == 0 && flag == 0) {
        stepper.disableOutputs();
        flag = 1;
    }
}
