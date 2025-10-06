```python
import socket
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

class AES256UDPSender:
    def __init__(self, ip, port, key, iv):
        self.ip = ip
        self.port = port
        self.key = key
        self.iv = iv

    def encrypt(self, message):
        cipher = AES.new(self.key, AES.MODE_CBC, self.iv)
        padded = pad(message.encode(), AES.block_size)
        return cipher.encrypt(padded)

    def send(self, message):
        encrypted = self.encrypt(message)
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            s.sendto(encrypted, (self.ip, self.port))
        print("Encrypted message (hex):", encrypted.hex().upper())

if __name__ == "__main__":
    key = bytes([
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    ])
    iv = bytes([
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    ])
    sender = AES256UDPSender("192.168.72.141", 4210, key, iv)
    sender.send("The code is working ")
```
