import serial
import time
import json
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

DEFAULT_PORT = "COM8"

def port_prompt():
    target_port = input("Please enter a port: ")
    if target_port == '' or target_port == None:
        return DEFAULT_PORT
    return target_port

arduino_port = port_prompt()
print("Starting Serial at " + arduino_port + "...")
arduino = serial.Serial(port = arduino_port)
time.sleep(0.5)

print("Starting audio services...")
devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
volume = cast(interface, POINTER(IAudioEndpointVolume))
    
def read_arduino():
    line = arduino.readline()   # read a byte
    if line:
        string = line.decode()  # convert the byte string to a unicode string

        data = json.loads(string)

        time.sleep(0.01)
        return data[0], data[1]
    return -1, -1
    
def send_arduino(pc_volume, is_mute):
    send_str = ""
    send_str += str(pc_volume) + "," + str(is_mute) + "\n"
    print("Sending to serial: " + send_str)
    arduino.write(send_str.encode("utf-8"))

def main():
    arduino_vol = -1
    arduino_mute = -1
    while True:
        time.sleep(0.001)

        curr_vol = round(volume.GetMasterVolumeLevelScalar() * 100)
        curr_mute = volume.GetMute()
        # print("Curr_vol" + str(curr_vol))
        while arduino.in_waiting:
            send_arduino(curr_vol, -1) 

            arduino_vol, arduino_mute = read_arduino()

            curr_mute = volume.GetMute()

        if arduino_vol != curr_vol or arduino_mute != curr_mute:
            send_arduino(curr_vol, 1 - curr_mute)
            arduino_vol = curr_vol
            arduino_mute = curr_mute

main()
