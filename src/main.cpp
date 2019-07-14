#include <keys.h>

#define STEPPIN 14
#define DIRPIN 4
#define EPIN 5
#define STEPS 575000

int flag = 0;
int boot = 0;
int lastPos = 0;

#include <RemoteMe.h>
#include <RemoteMeSocketConnector.h>
#include <ESP8266WiFi.h>
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, STEPPIN, DIRPIN);

RemoteMe& remoteMe = RemoteMe::getInstance(TOKEN, DEVICE_ID);

//*************** CODE FOR CONFORTABLE VARIABLE SET *********************

inline void setPosition(int32_t i) {remoteMe.getVariables()->setInteger("position", i); }

//*************** IMPLEMENT FUNCTIONS BELOW *********************

void onPositionChange(int32_t i) {

    if(boot != 0) {

        if (i == 1 && lastPos != 1) {
            stepper.move(-STEPS);
            lastPos = 1;
        }
        else if(i == 2 && lastPos != 2) {
            int halfStep = STEPS / 2;
            stepper.move(-halfStep);
            lastPos = 2;
        } 
        else if (i == 0 && lastPos != 0) {
            if (lastPos == 1) {
                stepper.move(STEPS);
                lastPos = 0;
            }
            else if(lastPos == 2) {
                int halfStep = STEPS / 2;
                stepper.move(halfStep);
                lastPos = 0;
            }
        }

        stepper.enableOutputs();
        stepper.setSpeed(10000);

        flag = 0;

    } else { 
        boot = 1;
    }
}

void setup() {
    stepper.setMaxSpeed(200000); 
    stepper.setAcceleration(10000);

    stepper.setEnablePin(EPIN);
    //stepper.setPinsInverted(DIRPIN);

    WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
    }

    remoteMe.getVariables()->observeInteger("position" ,onPositionChange);

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
