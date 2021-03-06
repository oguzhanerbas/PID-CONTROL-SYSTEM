// ALICI Son Hal

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte Adres[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char koordinat[10]; //OKUNAN KOORDİNAT DEGERLERİNİN TANIMLANMASI

LiquidCrystal_I2C lcd(0x27,20,4); 


void setup() {
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, Adres);
    radio.startListening();
}



void loop() {
  lcd.setCursor(0,0);     
  lcd.print("              ");  // CHAR DİZİSİNİN LCD DE ÇÖP DEĞER VERMEMESİ İÇİN
    getData();      
    showData();
    
}

// KOORDİNAT OKUMA FONKSİYONU
void getData() {
    radio.read( &koordinat, sizeof(koordinat) );
}

// KOORDİNAT LCD YAZDIRMA FONKSİYONU
void showData() {

    Serial.println(koordinat);
    lcd.setCursor(0,0);        //LCD EKRAN KOORDİNATI YAZDIRACAGIMIZ SATIR
    lcd.print("(");
    lcd.print(koordinat);
    lcd.print(")");
    delay(300);
    }
