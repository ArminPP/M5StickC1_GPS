/*

###########################
# M5Stack Unit GPS AT6558 #
###########################

SEE: 3.2 NMEA Extension in AT3340.ProductManual.pdf
'''''''''''''''''''''''''''''''''''''''''''''''''''

The changes can be stored permanetly in the internal flash.

$PCAS02,100*1E/r/n   			// 10 hz
$PCAS02,1000*2E/r/n 			// 1 Hz
$PCAS01,5*19/r/n   			    // Baud Rate 115200
$PCAS11,3*1E/r/n    		    // Dynamic Model: Automotive

$PCAS00*01/r/n					//save configuration	

PCAS03,1,1,1,1,1,1,0,0*02/r/n	// GGA,GGL,GSA,GSV,RMC,VTG=on,ZDA=off
$PCAS03,0,0,0,0,1,0,0,0*03/r/n	// RMC=on ONLY! (no $GPTXT,01,01,01,ANTENNA OPEN*25 ANYMORE !!!!!!)

#################
# U-BLOX NEO M7 #
#################

This device has no internal flash storage, always change the parameters during setup!


// TODO                          .
uBlox NEO 7M

B5 62 06 08 06 00 64 00 01 00 01 00 7A 12   // 10 hz


Additional this codes are also working:
BA CE 04 00 06 01 4E 00 00 00 52 00 06 01/r/n // GGA off
BA CE 04 00 06 01 4E 00 01 00 52 00 07 01/r/n // GGA on 

BA CE 04 00 06 01 4E 01 00 00 52 01 06 01/r/n // GLL off
BA CE 04 00 06 01 4E 02 00 00 52 02 06 01/r/n // GSA off

BA CE 04 00 06 01 4E 03 00 00 52 03 06 01/r/n // GSV off
BA CE 04 00 06 01 4E 05 00 00 52 05 06 01/r/n // VTG off
BA CE 04 00 06 01 4E 08 00 00 52 08 06 01/r/n // ZDA off
BA CE 04 00 06 01 4E 07 00 00 52 07 06 01/r/n // GSD off

BA CE 04 00 06 01 4E 04 00 00 52 04 06 01/r/n // RMC off
BA CE 04 00 06 01 4E 04 0A 00 52 04 10 01/r/n // RMC on 10x
BA CE 04 00 06 01 4E 04 01 00 52 04 07 01/r/n // RMC on 1x (high speed)  Recommended minimum navigation transmission data
*/


#include <Arduino.h>
#include <M5StickC.h>
#include <M5GFX.h>
#include "Free_Fonts.h"

#include "AXP192.h"
#include <TinyGPS++.h>

M5GFX TFT;
TinyGPSPlus GPS;
HardwareSerial GPS_SER(2);

const lgfx::GFXfont *speedFont = &fonts::FreeSansBold24pt7b;
const lgfx::GFXfont *unitFont  = &fonts::FreeSans9pt7b;
const lgfx::GFXfont *dateFont  = &fonts::FreeSans12pt7b;
const lgfx::GFXfont *timeFont  = &fonts::FreeSans12pt7b;

// forward declarations
void showGPSvalues();

void setup()
{
    M5.begin();

    Serial.begin(115200);
    GPS_SER.begin(9600, SERIAL_8N1, 32, 33);

    TFT.begin();
    TFT.setRotation(3);

    M5.Axp.EnableCoulombcounter(); // Enable Coulomb counter.
}

void loop()
{
    // TFT.setTextSize(1.5);
    // TFT.setCursor(0, 0);
    // TFT.printf("AXP Temp: %.1fC \r\n", M5.Axp.GetTempInAXP192());
    // TFT.printf("Bat:\r\n  V: %.3fv  I: %.3fma\r\n", M5.Axp.GetBatVoltage(), M5.Axp.GetBatCurrent());
    // TFT.printf("USB:\r\n  V: %.3fv  I: %.3fma\r\n", M5.Axp.GetVBusVoltage(), M5.Axp.GetVBusCurrent());
    // TFT.printf("5V-In:\r\n  V: %.3fv  I: %.3fma\r\n", M5.Axp.GetVinVoltage(), M5.Axp.GetVinCurrent());
    // TFT.printf("Bat power %.3fmw", M5.Axp.GetBatPower());

    while (GPS_SER.available() > 0)
    {
        if (GPS.encode(GPS_SER.read()))
        {
            showGPSvalues();
        }
    }

    if (millis() > 5000 && GPS.charsProcessed() < 10)
    {
        Serial.println(F("No GPS detected: check wiring."));
        while (true)
            ;
    }

    if (M5.Axp.GetVBusVoltage() < 2.5) // USB power is plugged off
    {
        TFT.fillScreen(RED);
        TFT.setTextFont(4);
        TFT.setTextSize(1);
        TFT.setTextColor(YELLOW, RED);
        for (int i = 5; i > 0; i--)
        {
            TFT.setCursor(20, 60);
            TFT.printf("Shut Down in %is ", i);

            for (int j = 0; j < 100; j++)
            {
                delay(10);
                if (M5.Axp.GetVBusVoltage() > 2.5)
                {
                    TFT.fillScreen(BLACK);
                    return;
                }
            }
        }

        M5.Axp.PowerOff();
    }
}

void showGPSvalues()
{
    static unsigned long displayLoopPM = 0;
    unsigned long displayLoopCM        = millis();
    if (displayLoopCM - displayLoopPM >= 100)
    {
        if (GPS.speed.isValid())
        {
            char speedKMH[5] = {'\0'};
            snprintf(speedKMH, sizeof(speedKMH), "%5.1f  ", GPS.speed.kmph());

            Serial.print(speedKMH);
            Serial.print(" | ");

            TFT.setTextSize(1.7);
            TFT.setTextColor(WHITE, BLACK);

            TFT.drawString(speedKMH, 1, 15, speedFont);
            TFT.setTextSize(1);
            TFT.drawString("km/h", 200, 65, unitFont);
        }
        else
        {
            Serial.println(F("INVALID"));
        }

        if (GPS.date.isValid())
        {
            char dt[15] = {'\0'};
            snprintf(dt, sizeof(dt), "%02i-%02i-%04i", GPS.date.day(), GPS.date.month(), GPS.date.year());
            Serial.print(dt);
            Serial.print(" | ");

            TFT.setTextSize(1);
            TFT.setTextColor(WHITE, BLACK);

            TFT.drawString(dt, 1, 100, dateFont);
        }
        else
        {
            Serial.print(F("INVALID"));
        }
        if (GPS.time.isValid())
        {
            char dt[15] = {'\0'};
            snprintf(dt, sizeof(dt), "%02i:%02i:%02i", GPS.time.hour(), GPS.time.minute(), GPS.time.second());
            Serial.print(dt);
            Serial.print(" | ");

            TFT.setTextSize(1);
            TFT.setTextColor(WHITE, BLACK);

            TFT.drawString(dt, 145, 100, timeFont);
        }
        else
        {
            Serial.print(F("INVALID"));
        }
        Serial.println();

        // -------- displayLoop end ---
        displayLoopPM = displayLoopCM;
    }
}