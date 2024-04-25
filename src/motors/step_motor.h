#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include <Arduino.h>

#define ENABLE PA0
#define STEP PB0
#define DIR PB1
#define MS1 PB3
#define MS2 PB4
#define MS3 PB5

enum StepMotorDirection {
    smdClockWise, 
    smdClockCounterWise
};

enum StepMotorSpeed {
    sms1_1 = 0,
    sms1_2 = 1,
    sms1_4 = 2,
    sms1_8 = 3,
    sms1_16 = 7
};

#define DEGREE_DEVIATION 5
#define MOTOR_WAIT 5

extern PACKET pack;
extern PACKET packTonR;

namespace stepM {
    double calculateAzimuth(double lat1, double lon1, double lat2, double lon2) {
        lat1 = lat1 * 3.14 / 180.0;
        lon1 = lon1 * 3.14 / 180.0;
        lat2 = lat2 * 3.14 / 180.0;
        lon2 = lon2 * 3.14 / 180.0;

        double dLon = lon2 - lon1;

        double y = sin(dLon) * cos(lat2);
        double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

        double azimuth = atan2(y, x);
        azimuth = azimuth * 180.0 / 3.14;

        if (azimuth < 0) {
            azimuth += 360;
        }

        return azimuth;
    }

    class StepMotor {
    private: 
        float stepsPerDegree;
    public:
        uint16_t stepsNeed;
        StepMotor() {
            stepsNeed = 0;

            pinMode(STEP, OUTPUT);
            pinMode(DIR, OUTPUT);
            pinMode(MS1, OUTPUT);
            pinMode(MS2, OUTPUT);
            pinMode(MS3, OUTPUT);
            pinMode(ENABLE, OUTPUT);
            
            digitalWrite(ENABLE, HIGH);
            setSpeed(sms1_4);
        }

        void setSpeed(StepMotorSpeed speed) {
            digitalWrite(MS1, speed >> 0 & 1);
            digitalWrite(MS2, speed >> 1 & 1);
            digitalWrite(MS3, speed >> 2 & 1);

            switch (speed) {
                case sms1_1: stepsPerDegree = 5.0f / 9.0f * 1; break;
                case sms1_2: stepsPerDegree = 5.0f / 9.0f * 2; break;
                case sms1_4: stepsPerDegree = 5.0f / 9.0f * 4; break;
                case sms1_8: stepsPerDegree = 5.0f / 9.0f * 8; break;
                case sms1_16: stepsPerDegree = 5.0f / 9.0f * 16; break;
            }        
        }

        void setDir(StepMotorDirection dir) {
            switch (dir) {
            case smdClockCounterWise:
                digitalWrite(DIR, HIGH);
                break;
            case smdClockWise:
                digitalWrite(DIR, LOW);
                break;
            }
        }

        void makeStep() {
            static uint32_t oldTime = 0;
            static uint32_t newTime;
            static bool stateStep = LOW;

            if (stepsNeed) {
                digitalWrite(ENABLE, LOW);
                newTime = millis();
                if (newTime - oldTime >= MOTOR_WAIT || newTime < oldTime) {
                    stateStep = !stateStep;
                    digitalWrite(STEP, stateStep);
                    if (stateStep == LOW) {
                        --stepsNeed;
                        if (stepsNeed == 0) {
                            digitalWrite(ENABLE, HIGH);
                        }
                    }
                    oldTime = newTime;
                }
            }
        }

        void angleRotationCalculation() {
        #if USE_MOTOR == 1 && TR_MODE == MODE_RECEIVER 
            static int16_t prev_angle = 0;
            static int16_t angle;
            angle = stepM::calculateAzimuth(pack.latitude, pack.longitude, packTonR.latitude, packTonR.longitude);
            if(abs(angle - prev_angle) > DEGREE_DEVIATION) {
                stepsNeed = (uint16_t) (abs(angle - prev_angle) * stepsPerDegree);
                setDir(angle - prev_angle < 0 ? smdClockCounterWise : smdClockWise);
                prev_angle = angle; 
            }
        #endif
        }
    };
}
#endif
