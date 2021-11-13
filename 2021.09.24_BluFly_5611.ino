#include <MS5611.h>

#include <Wire.h>                                 // Стандартная библиотека шины I2C
#include <MS5611.h>
MS5611 ms5611;
 //нормальным атмосферным давлением (101 325 Па).  18BCD

        #include <SoftwareSerial.h>                    // Библиотека интерфейса последовательной передачи
        SoftwareSerial BTSerial(8, 9); // RX | TX      // Указываем пины rx tx на который будет подключен Bluetooth модуль
        long P1;                                       // Переменная для вывода давления 

// переменные для калмана
float varVolt = 1.287386138;  // среднее отклонение (ищем в excel)
float varProcess = 0.01; // скорость реакции на изменение (подбирается вручную), 0.05=10-20см,  0.005=0-редко10, 0.01 почти как 0.005 но резвее
float Pc = 0.0;
float G = 0.0;
float P = 1.0;
float Xp = 0.0;
float Zp = 0.0;
float Xe = 0.0;
// переменные для калмана

//float pressure9;
        
void setup() {
         BTSerial.begin(9600);                         // Инициализация интерфейса передачи данных в монитор порта
  Serial.begin(115200);
  // Initialize MS5611 sensor
  // Ultra high resolution: MS5611_ULTRA_HIGH_RES
  // (default) High resolution: MS5611_HIGH_RES
  // Standard: MS5611_STANDARD
  // Low power: MS5611_LOW_POWER
  // Ultra low power: MS5611_ULTRA_LOW_POWER
  while(!ms5611.begin(MS5611_ULTRA_HIGH_RES))
  {
    delay(500);
  }
}
 
void loop() {
  /*/ Read true temperature & Pressure (without compensation)
  double realTemperature = ms5611.readTemperature();
  long realPressure = ms5611.readPressure();
  double realAltitude = ms5611.getAltitude(realPressure);*/

  // Read true temperature & Pressure (with compensation)
  //double realTemperature2 = ms5611.readTemperature(true);
  long realPressure2 = ms5611.readPressure(true);
  //double realAltitude2 = ms5611.getAltitude(realPressure2);

  // Output
  /*Serial.print(realTemperature);
  Serial.print(":");
  Serial.print(realTemperature2);
  Serial.print(":");
  Serial.print(realPressure);
  Serial.print(":");*/
  Serial.println(realPressure2);
  /*Serial.print(":");
  Serial.print(realAltitude);
  Serial.print(":");
  Serial.print(realAltitude2);
  Serial.println();*/

//for (int i = 0; i <= 49; i++) {

float fil_var = filter(realPressure2);

        P1 = round(fil_var);                    // Переводим показания в Па, округляем и сохраняем в целочисленном виде в P1

        BTSerial.print("PRS ");               // Передаем в Bluetooth модуль
        BTSerial.print(P1, HEX);              // в HEX виде
        BTSerial.print("\n");                 // Например: "PRS 1817F\n"
     

  }

float filter(float val) {  //функция фильтрации
  Pc = P + varProcess;
  G = Pc/(Pc + varVolt);
  P = (1-G)*Pc;
  Xp = Xe;
  Zp = Xp;
  Xe = G*(val-Zp)+Xp; // "фильтрованное" значение
  return(Xe);
}
