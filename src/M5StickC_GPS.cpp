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

$PCAS03,1,1,1,1,1,1,0,0*02/r/n	// GGA,GGL,GSA,GSV,RMC,VTG=on,ZDA=off
$PCAS03,0,0,0,0,1,0,0,0*03/r/n	// RMC=on ONLY! (no $GPTXT,01,01,01,ANTENNA OPEN*25 ANYMORE !!!!!!)


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
test.xx_cppppc LINE 938ff is some code !!!!!!!!!!!!!!!!!!!
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''



#################
# U-BLOX NEO M7 #
#################

This device has no internal flash storage, always change the parameters during setup!
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

const unsigned char ubxRate1Hz[] PROGMEM = { 0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00 };
const unsigned char ubxRate5Hz[] PROGMEM = { 0x06,0x08,0x06,0x00,200,0x00,0x01,0x00,0x01,0x00 };
const unsigned char ubxRate10Hz[] PROGMEM ={ 0x06,0x08,0x06,0x00,100,0x00,0x01,0x00,0x01,0x00 };

const uint8_t RXM_MAXP[] PROGMEM = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00, 0x21, 0x91}; // Max Performance Mode
const uint8_t RXM_PSM[] PROGMEM  = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01, 0x22, 0x92}; // Power Save Mode


The $PMTK commands are for the MTK chip.
All ublox NEO configuration commands start with $PUBX (NMEA text commands).
There are additional configuration options you can set with UBX binary commands.

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

#ifdef UBLOX
const byte ClearConfig[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x19, 0x98};
const byte GPGLLOff[]    = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B};
const byte GPGSVOff[]    = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39};
const byte GPVTGOff[]    = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47};
const byte GPGSAOff[]    = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32};
const byte GPGGAOff[]    = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x24};
const byte GPRMCOff[]    = {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40};
const byte Navrate10hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};
const byte baud115200[]  = {0xb5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x08, 0x00, 0x00, 0x00, 0xc2, 0x01,
                            0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x96, 0xb5, 0x62, 0x06, 0x00, 0x01};
const byte RXM_MAXP[]    = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00, 0x21, 0x91}; // Max Performance Mode

#else

const char *Navrate10hz     = "$PCAS02,100*1E\r\n";             // 10 hz
const char *Navrate1hz      = "$PCAS02,1000*2E\r\n";            // 1 Hz
const char *baud115200      = "$PCAS01,5*19\r\n";               // Baud Rate 115200
const char *ModelAutomotive = "$PCAS11,3*1E\r\n";               // Dynamic Model: Automotive
const char *ModeZDA         = "$PCAS03,1,1,1,1,1,1,0,0*02\r\n"; // GGA,GGL,GSA,GSV,RMC,VTG=on,ZDA=off ZDA=UTCtime,day,month,year,ltzh,ltzn*CS
const char *ModeRMC         = "$PCAS03,0,0,0,0,1,0,0,0*03\r\n"; // RMC=on ONLY! (no $GPTXT,01,01,01,ANTENNA OPEN*25 ANYMORE !)
                                                                // RMC=UTCtime,status,lat,uLat,lon,uLon,spd,cog,date,mv,mvE,mode*CS
const char *SaveConfig = "$PCAS00*01\r\n";                      // save configuration

#endif

// forward declarations
void showGPSvalues();
void sendPacket(const byte *packet, byte len);

void setup()
{
    M5.begin();

    Serial.begin(115200);
    TFT.begin();
    TFT.setRotation(3);

    M5.Axp.EnableCoulombcounter(); // Enable Coulomb counter.

    delay(2000); // wait, if car is started ...

#ifdef UBLOX
    Serial.println("GNSS: starting with 9600 baud");
    GPS_SER.begin(9600, SERIAL_8N1, 32, 33);

    sendPacket(baud115200, sizeof(baud115200));
    delay(100);

    Serial.println("GNSS: set to 115200 baud");
    GPS_SER.updateBaudRate(115200);
    delay(100);

    Serial.println("GNSS: disable not needed messages");
    sendPacket(GPGLLOff, sizeof(GPGLLOff));
    delay(100);
    sendPacket(GPGSVOff, sizeof(GPGSVOff));
    delay(100);
    sendPacket(GPVTGOff, sizeof(GPVTGOff));
    delay(100);
    sendPacket(GPGSAOff, sizeof(GPGSAOff));
    delay(100);
    sendPacket(GPGGAOff, sizeof(GPGGAOff));
    delay(100);
    sendPacket(Navrate10hz, sizeof(Navrate10hz));
    delay(100);
    sendPacket(RXM_MAXP, sizeof(RXM_MAXP));
    delay(100);
#else
    Serial.println("GNSS: starting with 9600 baud");
    GPS_SER.begin(9600, SERIAL_8N1, 33, 32); // Wires crossed!

    GPS_SER.print(baud115200);
    delay(100);

    Serial.println("GNSS: set to 115200 baud");
    GPS_SER.updateBaudRate(115200);
    delay(100);

    Serial.println("GNSS: disable not needed messages");
    GPS_SER.print(ModeRMC);
    delay(100);

    Serial.println("GNSS: 10Hz Nav Rate");
    GPS_SER.print(Navrate10hz);
    delay(100);

    Serial.println("GNSS: Dynamic Model: Automotive");
    GPS_SER.print(ModelAutomotive);
    delay(100);

    Serial.println("GNSS: save configuration");
    GPS_SER.print(SaveConfig);
    delay(100);

#endif
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
        char c = GPS_SER.read();
        Serial.write(c); // write raw data from GPS to console
        if (GPS.encode(c))
        {
            showGPSvalues();
        }
    }

    if (millis() > 5000 && GPS.charsProcessed() < 10)
    {
        Serial.println(F("------------------------> No GPS detected: check wiring."));
        // while (true)
        //     ;
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

void sendPacket(const byte *packet, byte len)
{
    Serial.print("\r\n-----------> Packet sent to u-blox: ");
    for (byte i = 0; i < len; i++)
    {
        GPS_SER.write(packet[i]);

        Serial.print(packet[i], HEX);
        Serial.print(F(" "));
    }
    Serial.println();
}

void showGPSvalues()
{
    static unsigned long displayLoopPM = 0;
    unsigned long displayLoopCM        = millis();
    if (displayLoopCM - displayLoopPM >= 100)
    {
        if (GPS.speed.isValid())
        {
            char speedKMH[6] = {'\0'};
            snprintf(speedKMH, sizeof(speedKMH), "%5.1f  ", GPS.speed.kmph());
            // snprintf(speedKMH, sizeof(speedKMH), "%5.1f  ", 188.8);

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