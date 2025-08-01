% MATLAB code to send AES-256 encrypted messages to ESP32 using Java

import java.net.DatagramSocket
import java.net.DatagramPacket
import java.net.InetAddress

% Define the ESP32 IP address and port
ipAddress = '192.168.72.141';  % Replace with ESP32's IP
port = 4210;  % Same as in the ESP32 code

% Define the message you want to send
message = 'The code is working ';

% AES-256 key (256-bit)
key = uint8([0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, ...
             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, ...
             0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, ...
             0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F]);  % 256-bit key

% Initialization vector (IV) for AES (16 bytes)
iv = uint8([0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, ...
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F]);  % 16-byte IV

% Convert message to uint8
messageBytes = uint8(message);

% Pad message to multiple of 16 bytes (AES block size)
padLength = 16 - mod(length(messageBytes), 16);
messagePadded = [messageBytes, uint8(repmat(padLength, 1, padLength))];

% Encrypt the message using AES-256 in CBC mode
cipher = javax.crypto.Cipher.getInstance('AES/CBC/NoPadding');
keySpec = javax.crypto.spec.SecretKeySpec(key, 'AES');
ivSpec = javax.crypto.spec.IvParameterSpec(iv);
cipher.init(javax.crypto.Cipher.ENCRYPT_MODE, keySpec, ivSpec);
encryptedMessage = cipher.doFinal(messagePadded);

% Create a UDP socket using java.net.DatagramSocket
udpSocket = DatagramSocket();

% Convert IP address to Java InetAddress
inetAddress = InetAddress.getByName(ipAddress);

% Create a UDP packet with the encrypted message
packet = DatagramPacket(encryptedMessage, length(encryptedMessage), inetAddress, port);

% Send the packet via UDP
udpSocket.send(packet);

disp('Encrypted message sent.');
disp(encryptedMessage);

% Close the socket after use
udpSocket.close();
