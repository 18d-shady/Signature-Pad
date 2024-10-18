import base64
from PIL import Image
from datetime import datetime
#from StringIO import StringIO
import serial
import serial.tools.list_ports
#import struct
import array
from tkinter.filedialog import asksaveasfile
import numpy as np
import cv2
import pyautogui


width = 210
height = 320
toggleScreen = 0
confirm = ''
#ser.enableRx(true)

comlist =  serial.tools.list_ports.comports()

while comlist:
   for i in comlist:
      print(i.device)
      try:
         ser = serial.Serial(i.device, baudrate=115200, timeout=1)
         for i in range(3):
            confirm = ser.readline()
            print(confirm)
            if confirm != b"":
               break

         if confirm == b"SPad connecting to PC..":
            ser.write(b'PC connected to SPad')
            print("connected")
            break
         else:
            ser.close()
      except serial.serialutil.SerialException:
         pass
      
   if confirm == b"SPad connecting to PC..":
      print("leaving")
      break

#ser = serial.Serial("COM3", baudrate=115200, timeout=1)
#ser.write(b'PC connected to SPad')
ser.readall()
print("hey")
while True:
   initial = ser.readline()
   if initial != b'':
      print(initial)
      if initial == b'pdScreen':
         toggleScreen = 0
         break
         
      elif initial == b'pcScreen':
         toggleScreen = 1
         break


def secondOption():
   global toggleScreen
   
   img = np.zeros((width, height, 3), np.uint8)
   # blue channel
   img[:,:,0]=255
   # green channel
   img[:,:,1]=255
   # red channel
   img[:,:,2]=255


   while True:
      data = ser.read(ser.in_waiting)
      if data:
         if data == b"pcResign":
            secondOption()
         elif data == b"donePcSave":
            print("Save")
            curr_datetime = datetime.now().strftime('%Y-%m-%d %H-%M-%S')
            f = asksaveasfile(initialfile="signature " + curr_datetime + ".png", defaultextension=".png", filetypes=(("PNG Files", "*.png"), ("JPEG Files", "*.jpg")))
            if f is None:  # if user cancels, return
               cv2.imwrite("signature " + curr_datetime + ".png", img)

            else:
               # you can write to the file here
               cv2.imwrite(f.name, img)
            toggleScreen = 0
            ser.readall()
            break
            
         elif data == b"pdScreen":
            print("pdScreen")
            toggleScreen = 0
            ser.readall()
            break
         
            
         else:
            #print(data)
            ba = bytearray(data)
            if len(ba) % array.array('I', b'').itemsize != 0:
               continue
            arr = array.array('I', ba)
            arr_l = len(arr)
            #print(arr)
            
            
            for coord in range(0, arr_l, 2):
               cv2.circle(img, (arr[coord], arr[coord+1]), 1, (0, 0, 0), cv2.FILLED)
              

               # display image
               cv2.imshow("New Signature", img)


      # This function will make sure that the 
      # image is opened until the user presses esc 
      if cv2.waitKey(5) & 0xFF == 27:
         break

   # After that, we close all the window
   print("breaking...")
   toggleScreen = 0
   cv2.destroyAllWindows()


def firstOption():
   global toggleScreen
   data = ser.readline()
   if data != b'':
      print(data)
      if data == b'pcScreen':
         toggleScreen = 1
      elif data == b'pdScreen':
         pass
      else:
         #curr_datetime = datetime.now().strftime('%Y-%m-%d %H-%M-%S')
         try:
            image = Image.frombytes("RGB", (width, height), data)
         except ValueError:
            print("ValueError")
            pass
         except TypeError:
            print("TypeError")
            pass
         image = image.transpose(Image.ROTATE_270)
         curr_datetime = datetime.now().strftime('%Y-%m-%d %H-%M-%S')
         f = asksaveasfile(initialfile="signature " + curr_datetime + ".png", defaultextension=".png", filetypes=(("PNG Files", "*.png"), ("JPEG Files", "*.jpg")))
      
         if f is None:  # if user cancels, return
            image.save("signature " + curr_datetime + ".png")
            #return
         else:
            # you can write to the file here
            image.save(f.name)
            #print(f.name)
            #f.close()
            #image.save("signature " + curr_datetime + ".png")
            #image.show()

            
         


while True:
   if toggleScreen == 0:
      firstOption()
   elif toggleScreen == 1:
      secondOption()
   print(toggleScreen)
   
#array_size = 2 * struct.calcsize('<i')
#array_data = ser.read(ser.in_waiting)
#if array_data:
   #if array_data == b'pdScreen':
      #toggleScreen = 0
      #break
   #coord = struct.unpack('<ii', array_data)
   #ser.flush()   
       
