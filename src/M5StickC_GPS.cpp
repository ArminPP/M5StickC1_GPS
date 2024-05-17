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
            snprintf(speedKMH, sizeof(speedKMH), "%4.1f ", GPS.speed.kmph());

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

            TFT.drawString(dt, 10, 100, dateFont);
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