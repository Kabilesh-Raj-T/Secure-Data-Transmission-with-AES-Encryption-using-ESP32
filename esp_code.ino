#include <WiFi.h>
#include <WiFiUdp.h>
#include <mbedtls/aes.h>

const char* ssid = "Xiaomi 11i";
const char* password = "12345678";

WiFiUDP udp;
WiFiServer server(80);
unsigned int localUdpPort = 4210;
char incomingPacket[255];
String decryptedMessage = "No message received yet.";

unsigned char aes_key[32] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
  0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F
};
unsigned char iv[16] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  
  udp.begin(localUdpPort);
  server.begin();
  Serial.println("WiFi connected. Listening on UDP port " + String(localUdpPort));
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;

    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, 256);

    unsigned char decrypted[255];
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, len, iv_copy, (unsigned char*)incomingPacket, decrypted);

    int padding_len = decrypted[len - 1];
    if (padding_len > 16) padding_len = 0;
    decrypted[len - padding_len] = 0;
    decryptedMessage = String((char*)decrypted);

    Serial.println("Decrypted message: " + decryptedMessage);
    mbedtls_aes_free(&aes);
  }

  WiFiClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        if (line.length() == 1) { 
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("<html><body style='text-align:center'>");
          client.println("<h1>DECRYPTED UDP MESSAGE</h1>");
          client.println("<h2>" + decryptedMessage + "</h2>");
          client.println("</body></html>");
          break;
        }
      }
    }
    client.stop();
  }
}
