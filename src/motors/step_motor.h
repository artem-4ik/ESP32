#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include <Arduino.h>

#define ENABLE PA0
#define STEP PB0
#define DIR PB1
#define MS1 PB3
#define MS2 PB4
#define MS3 PB5

#define D_RIGHT 1
#define D_LEFT -1
#define DEGREE_DEVIATION 5

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
            float step_range;
            uint8_t wait;
        public:
            uint16_t step_count;
            uint8_t direction;
            
            StepMotor() {
                step_range = 1.8;
                wait = 16;
                direction = D_RIGHT;
                step_count = 0;
            };

            void start() {
                pinMode(STEP, OUTPUT);
                pinMode(DIR, OUTPUT);
                pinMode(MS1, OUTPUT);
                pinMode(MS2, OUTPUT);
                pinMode(MS3, OUTPUT);
                pinMode(ENABLE, OUTPUT);

                digitalWrite(ENABLE, HIGH);
            };

            void AngleToStep(int angle) {
                step_count = round(angle / step_range);
            };

            void controlMS1(bool c) {
                if(c) {
                    digitalWrite(MS1, HIGH);
                }
                else {
                    digitalWrite(MS1, LOW);
                }
            };

            void controlMS2(bool c) {
                if(c) {
                    digitalWrite(MS2, HIGH);
                }
                else {
                    digitalWrite(MS2, LOW);
                }
            };

            void controlMS3(bool c) {
                if(c) {
                    digitalWrite(MS3, HIGH);
                }
                else {
                    digitalWrite(MS3, LOW);
                }
            };

            void controlDIR(uint8_t dir) {
                switch (dir) {
                case D_RIGHT:
                    digitalWrite(DIR, HIGH);
                    direction = D_RIGHT;
                    break;
                case D_LEFT:
                    digitalWrite(DIR, LOW);
                    direction = D_LEFT;
                    break;

                default:
                    break;
                }
            }

            void steps() {
                digitalWrite(ENABLE, LOW);

                for(int i = 0; i < step_count; i++) {
                    digitalWrite(STEP, HIGH);
                    delay(wait);
                    digitalWrite(STEP, LOW);
                    delay(wait);
                }
                
                digitalWrite(ENABLE, HIGH);

                step_count = 0;
            }
    };
}

#endif
