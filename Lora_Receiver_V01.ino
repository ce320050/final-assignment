#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

String inputString;       //digunakan untuk menyimpan data yang diterima melalui LoRa.
String strs[20];          //array yang digunakan untuk menyimpan token-token dari data yang diterima.
int StringCount = 0;      //menghitung jumlah token yang telah dipisahkan.

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

}

void loop() {
   int packetSize = LoRa.parsePacket();    //memeriksa apakah ada paket data yang diterima melalui LoRa.
  if (packetSize) {
    String message = "";                    //data yang di terima di simpan ke message
    while (LoRa.available()) {
      message += (char)LoRa.read();
    }
    
    // Memisahkan data menjadi token berdasarkan koma
    String tokens[4]; 
    int tokenIndex = 0;
    int lastIndex = 0;
    for (int i = 0; i < message.length(); i++) {
      if (message.charAt(i) == ',') {
        tokens[tokenIndex++] = message.substring(lastIndex, i);
        lastIndex = i + 1;
      }
    }
    tokens[tokenIndex] = message.substring(lastIndex);

    // Memeriksa jumlah token yang sesuai
    if (tokenIndex == 3) {
      int suhu = tokens[0].toInt();
      int ntu = tokens[1].toInt();
      int ph = tokens[2].toInt();
      int Do = tokens[3].toInt();

      // Membuat objek JSON
      StaticJsonDocument<100> doc;
      doc["suhu"] = suhu;
      doc["ntu"] = ntu;
      doc["ph"] = ph;
      doc["do"] = Do;

      // Mengonversi objek JSON menjadi string JSON
      String json;
      serializeJson(doc, json);

      // Mengirimkan string JSON melalui Serial ke ESP32
      Serial.println(json);
    }

  }
}
