import time
import serial
import serial.tools.list_ports


class SerialManager:
    def __init__(self, baudrate, port=None, timeout=0.2):
        self.serial: serial.Serial = None
        self.port = None
        self.baudrate = baudrate
        self.timeout = timeout
        if not port is None:
            self.port = port
            self.connect()
    def connect(self):
        self.serial = serial.Serial(self.port, self.baudrate, timeout=self.timeout)
        time.sleep(1)
    def get_available_ports(self):
        return [i.name for i in serial.tools.list_ports.comports()]
    def auto_connect(self):
        ports = self.get_available_ports()
        if len(ports) == 1:
            self.port = ports[0]
        else:
            self.port = input("Choose a port to connect:\n"+str(ports)+"\n\t>>")
        self.connect()
    def send(self, msg: str):
        self.serial.write(msg.encode('utf-8'))
        print("sending: "+msg)
    def read(self):
        txt = self.serial.readline().decode('utf-8')
        if txt:
            return txt
        return ""


if __name__ == "__main__":
    serial_manager = SerialManager(115200)
    serial_manager.auto_connect()
