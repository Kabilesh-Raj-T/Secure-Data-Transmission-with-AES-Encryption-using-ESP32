#include <WiFi.h>
#include <WiFiUdp.h>
#include <mbedtls/aes.h>  // Official ESP32 library for AES

const char* ssid = "Xiaomi 11i";    // Your WiFi name
const char* password = "12345678";  // Your WiFi password

WiFiUDP udp;
WiFiServer server(80);              // Web server running on port 80
unsigned int localUdpPort = 4210;    // Listening port
char incomingPacket[255];            // Buffer for incoming messages
String decryptedMessage = "No message received yet.";  // Default message

// AES key (256 bits) and IV (128 bits)
unsigned char aes_key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                             0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                             0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F}; // 32 bytes (256-bit key)

unsigned char iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F}; // 16 bytes IV

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start listening for UDP messages
  udp.begin(localUdpPort);
  Serial.printf("Listening on UDP port %d\n", localUdpPort);

  // Start the web server
  server.begin();
}

void loop() {
  // Check for incoming UDP messages
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;  // Null-terminate the received message
    }

    // Reset the IV before each decryption (as it's CBC mode)
    unsigned char iv_copy[16];
    memcpy(iv_copy, iv, 16);  // Copy the original IV

    // Decrypt the message using AES-256 in CBC mode
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aes_key, 256);

    unsigned char decrypted[255];  // Buffer for decrypted message

    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, len, iv_copy, (unsigned char*)incomingPacket, decrypted);
    
    // Remove padding (assuming PKCS#7 padding)
    int padding_len = decrypted[len - 1];  // Last byte contains padding length
    if (padding_len > 16) {
      padding_len = 0;  // In case of incorrect padding, prevent removal
    }
    int decrypted_len = len - padding_len;
    
    decrypted[decrypted_len] = 0;  // Null-terminate decrypted string

    // Store the decrypted message
    decryptedMessage = String((char*)decrypted);
    Serial.printf("Decrypted message: %s\n", decryptedMessage.c_str());

    mbedtls_aes_free(&aes);
  }

  // Handle web client requests
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // If the current line is blank, it means the request is complete
          if (currentLine.length() == 0) {
            // Send a HTTP response with the decrypted message
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<html><head>");
            client.print("</head><body>");
            client.print("<h1 style=\"text-align:center;\">DECRYPTED UDP MESSAGE</h1>");
            client.print("<h2 style=\"text-align:center;\">");
            client.print(decryptedMessage);  // Display decrypted message
            client.print("</h2>");
            client.println("</body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}
