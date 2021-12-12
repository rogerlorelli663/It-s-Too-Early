#!/usr/bin/env python3
import datetime
import sys
import os
import serial

def check(int1, int2):
    if 15 > int1 > 1023:
        int1 = 0
    if 15 > int2 > 1023:
        int2 = 0
    if int1 < int2:
        return int2, int1
    else:
        return int1, int2

upthreshold = int(sys.argv[1])
lowthreshold = int(sys.argv[2])

upthreshold, lowthreshold = check(upthreshold, lowthreshold)

date_time = datetime.datetime.now()

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=5)

with open("/var/www/html/threshold_interface.html", "r") as html_file:
    content = html_file.readlines()

    for i in range(len(content)):
        content[i] = content[i].strip()

    if upthreshold == 0:
        #print(f'Old: {upthreshold} New: {content[len(content) - 12]}')
        upthreshold = content[len(content) - 12]
    if lowthreshold == 0:
        #print(f'Old: {lowthreshold} New: {content[len(content) - 9]}')
        lowthreshold = content[len(content) - 9]
    
    index = content.index('</table>')
    content.insert(index, '<tr>')
    content.insert(index+1, '<td>')
    content.insert(index+2, f'{upthreshold}')
    content.insert(index+3, '</td>')
    content.insert(index+4, '<td>')
    content.insert(index+5, f'{lowthreshold}')
    content.insert(index+6, '</td>')
    content.insert(index+7, '<td>')
    content.insert(index+8, f'{date_time.strftime("%Y-%m-%d %H:%M:%S.%f")}')
    content.insert(index+9, '</td>')
    content.insert(index+10, '</tr>')

    with open("/var/www/html/threshold_interface.html", "w") as html_file:
        for line in content:
            html_file.write(line)
            html_file.write('\n')
upthreshold = str(upthreshold) + " " + str(lowthreshold)
line = ''
confirmation = f'I received: {upthreshold}'
while line != confirmation:
    ser.write(bytes(upthreshold.encode('utf-8')))
    line = ser.readline().decode('utf-8').rstrip()
    print(line)
    print(type(line))
    print(type(upthreshold))
    if line == upthreshold:
        print('Should stop')

print("done")
