import csv
import time

import serial

ser = serial.Serial('COM3',baudrate=9600)

while True:
    try:
        ser_bytes= ser.readline().decode('ascii')
        print(ser_bytes)
        data_split=ser_bytes.split(",")
        with open("thrust setup 27_02_2020 test3_avioncs_2830kv_1105.csv","a") as f:
            writer = csv.writer(f,delimiter=",")
            writer.writerow([time.time(),data_split[0],data_split[1],data_split[2],data_split[3][0:len(data_split[3])-2]])
    except:
        print("Keyboard Interrupt")
        break
