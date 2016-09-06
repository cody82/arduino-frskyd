/*
 * Requirements
 * ------------
 * - FrskyD library - https://github.com/jcheger/frsky-arduino
 * 
 * - Streaming library - http://arduiniana.org/libraries/streaming/
 *                       http://arduiniana.org/Streaming/Streaming5.zip
 *
 * 
 * origin: https://github.com/jcheger/frsky-arduino
 * author: Jean-Christophe Heger <jcheger@ordinoscope.net>
 */
 
#include <FrskyD.h>
#include <SoftwareSerial.h>
#include <Streaming.h>

FrskyD FrskyD (10, 11);

void setup() {
  Serial.begin (115200);
  Serial.println ("FrskyD sniffer");
}

void loop () {
  static byte buffer[100];
  static int idx = 0;
  int i;

  while (FrskyD.available ()) {
    buffer[idx] = FrskyD.read ();
    if (buffer[idx-1] == 0x5E && buffer[idx] == 0x5E) {
      Serial << "\n" << "--- [FRAME] --- ";
      for (i = 0; i < idx; i++) Serial << _HEX(buffer[i]) << " ";
      Serial << " (" << idx << ")" << endl;
      decode_frame (buffer, idx);
      buffer[0] = 0x5E;
      idx = 1;
    } else {
      idx++;
      if (idx > 100) {  // error
        buffer[0] = 0xE;
        idx = 1;
      }
    }
  }
}

void decode_frame (byte *buffer, int length) {
  int i = 1;
  
  int16_t        alt_a, gps_alt_a, gps_course_a, gps_lat_a, gps_long_a, gps_speed_a, voltage_a;
  static int16_t alt_b, gps_alt_b, gps_course_b, gps_lat_b, gps_long_b, gps_speed_b, voltage_b;
  
  while (i < length) {

    switch (buffer[i]) {

      case FRSKY_D_ACCX:         Serial << "AccX:       " << FrskyD.decodeInt (&buffer[i+1]) / 1000.0 << " [g]" << endl; break;
      case FRSKY_D_ACCY:         Serial << "AccY:       " << FrskyD.decodeInt (&buffer[i+1]) / 1000.0 << " [g]" << endl; break;
      case FRSKY_D_ACCZ:         Serial << "AccZ:       " << FrskyD.decodeInt (&buffer[i+1]) / 1000.0 << " [g]" << endl; break;

      case FRSKY_D_ALT_B:        alt_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "--- skip GPS_ALT_B" << endl;
                                 break;
      case FRSKY_D_ALT_A:        alt_a = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "Alt:        " << FrskyD.calcFloat (alt_b, alt_a) << " [m]" << endl;
                                 break;

      case FRSKY_D_CELL_VOLT:    Serial << "CellV[" << FrskyD.decodeCellVoltId (&buffer[i+1]) << "]:   " << FrskyD.decodeCellVolt (&buffer[i+1]) << " [V]" << endl; break;

      case FRSKY_D_FUEL:         Serial << "Fuel:       " << FrskyD.decodeInt (&buffer[i+1]) << " [%]" << endl; break;
 
      case FRSKY_D_GPS_ALT_B:    gps_alt_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "--- skip GPS_ALT_B" << endl;
                                 break;
      case FRSKY_D_GPS_ALT_A:    gps_alt_a = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "GpsAlt:     " << FrskyD.calcFloat (gps_alt_b, gps_alt_a) << " [m] << endl";
                                 break;
 
      case FRSKY_D_GPS_COURSE_B: gps_course_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "--- skip GPS_COURSE_B" << endl;
                                 break;
      case FRSKY_D_GPS_COURSE_A: gps_course_a = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "GpsCourse:  " << FrskyD.calcFloat (gps_course_b, gps_course_a) << " [" << char(176) << "]" << endl;
                                 break;
 
      case FRSKY_D_GPS_DM:       Serial << "Day, Month: " << FrskyD.decode1Int (&buffer[i+1]) << " " << FrskyD.decode1Int (&buffer[i+2]) << endl; break;
      case FRSKY_D_GPS_HM:       Serial << "Hour, Min:  " << FrskyD.decode1Int (&buffer[i+1]) << " " << FrskyD.decode1Int (&buffer[i+2]) << endl; break;
 
      case FRSKY_D_GPS_LAT_B:    gps_lat_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "--- skip GPS_LAT_B" << endl;
                                 break;
      case FRSKY_D_GPS_LAT_A:    gps_lat_a = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "GpsLat:     " << FrskyD.decodeGpsLat (gps_lat_b, gps_lat_a) << endl;
                                 break;
      
      case FRSKY_D_GPS_LAT_NS:   Serial << "GpsLatNS:   " << FrskyD.decodeInt (&buffer[i+1]) << endl; break;

      case FRSKY_D_GPS_LONG_B:   gps_long_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "--- skip GPS_LONG_B" << endl;
                                 break;
      case FRSKY_D_GPS_LONG_A:   gps_long_a = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "GpsLong:    " << FrskyD.decodeGpsLong (gps_long_b, gps_long_a) << endl;
                                 break;
      
      case FRSKY_D_GPS_LONG_EW:  Serial << "GpsLongEW:  " << FrskyD.decodeInt (&buffer[i+1]) << endl; break;
      case FRSKY_D_GPS_SEC:      Serial << "Sec:        " << FrskyD.decodeInt (&buffer[i+1]) << endl; break;

      case FRSKY_D_GPS_SPEED_B:  gps_speed_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "--- skip GPS_SPEED_B" << endl;
                                 break;
      case FRSKY_D_GPS_SPEED_A:  gps_speed_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "GpsSpeed:   " << FrskyD.calcFloat (gps_speed_b, gps_speed_a) << " [knots]" << endl;
                                 break;

      case FRSKY_D_GPS_YEAR:     Serial << "Year:       " << FrskyD.decodeInt (&buffer[i+1]) << endl; break;
      case FRSKY_D_RPM:          Serial << "Rpm:        " << FrskyD.decodeInt (&buffer[i+1]) << " [rpm]" << endl; break;
      case FRSKY_D_TEMP1:        Serial << "Temp1:      " << FrskyD.decodeInt (&buffer[i+1]) << " [" << char(176) << "C]" << endl; break;
      case FRSKY_D_TEMP2:        Serial << "Temp2:      " << FrskyD.decodeInt (&buffer[i+1]) << " [" << char(176) << "C]" << endl; break;
      case FRSKY_D_CURRENT:      Serial << "Current:    " << FrskyD.decodeInt (&buffer[i+1]) << " [A]" << endl; break;

      case FRSKY_D_VFAS:         Serial << "VFAS:       " << FrskyD.decodeInt (&buffer[i+1]) / 10 << " [V]" << endl; break;

      case FRSKY_D_VOLTAGE_A:    voltage_a = FrskyD.decodeInt (&buffer[i+1]);;
                                 Serial << "Voltage A:  " << (float) (voltage_b * 10 + voltage_a) * 21 / 110 << " [V]" << endl;
                                 break;

      case FRSKY_D_VOLTAGE_B:    voltage_b = FrskyD.decodeInt (&buffer[i+1]);
                                 Serial << "Voltage B:  " << (float) (voltage_b * 10 + voltage_a) * 21 / 110 << " [V]" << endl;
                                 break;
      
      default:
        Serial << "unknown ID:    " << _HEX(buffer[i]) << endl;
        Serial << "decodeInt:     " << FrskyD.decodeInt (&buffer[i+1]) << endl;
        Serial << "decode1Int[0]: " << FrskyD.decodeInt (&buffer[i+1]) << endl;
        Serial << "decode1Int[1]: " << FrskyD.decodeInt (&buffer[i+2]) << endl;
        Serial.print ("HEX: ");
        while (buffer[i] != 0x5E) { Serial << _HEX(buffer[i]) << " "; i++; }
        i--;
        Serial << endl;
    }
    i++;
    while (buffer[i-1] != 0x5E) i++;
  }
}
