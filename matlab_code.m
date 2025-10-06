import java.net.DatagramSocket
import java.net.DatagramPacket
import java.net.InetAddress
import javax.crypto.Cipher
import javax.crypto.spec.SecretKeySpec
import javax.crypto.spec.IvParameterSpec

ipAddress = '192.168.72.141';
port = 4210;
message = 'The code is working ';

key = uint8(0:31);
iv  = uint8(0:15);

messageBytes = uint8(message);
blockSize = 16;
padLength = blockSize - mod(numel(messageBytes), blockSize);
if padLength == 0
    padLength = blockSize;
end
messagePadded = [messageBytes, repmat(uint8(padLength), 1, padLength)];

cipher = Cipher.getInstance('AES/CBC/NoPadding');
keySpec = SecretKeySpec(key, 'AES');
ivSpec = IvParameterSpec(iv);
cipher.init(Cipher.ENCRYPT_MODE, keySpec, ivSpec);
encryptedMessage = cipher.doFinal(messagePadded);

udpSocket = DatagramSocket();
inetAddress = InetAddress.getByName(ipAddress);
packet = DatagramPacket(encryptedMessage, numel(encryptedMessage), inetAddress, port);
udpSocket.send(packet);

disp('Encrypted message sent.');
disp(encryptedMessage.');

udpSocket.close();
