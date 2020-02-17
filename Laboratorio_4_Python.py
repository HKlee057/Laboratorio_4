#Laboratorio 4 - Electr�nica Digital 2
#Hector A. Kl�e Gonz�lez
#17118


from Tkinter import *
import serial
import time
import sys

#Definici�n de variables
volt_POT1 = 0.00
volt_POT2 = 0.00
real_POT1 = 0
real_POT2 = 0

#Configuraci�n del Puerto Serial
puerto = 'COM7'
ser = serial.Serial(port = puerto, baudrate = 9600,
                    parity = serial.PARITY_NONE,
                    stopbits = serial.STOPBITS_ONE,
                    bytesize = serial. EIGHTBITS, timeout = 0)


#Configuraci�n de la ventana
ventana = Tk()
ventana.title("Laboratorio 4: Comunicaci�n UART y SPI")
ventana.geometry("500x285")
ventana.config(bg='firebrick4')

name = Label(ventana, text="Hector A. Kl�e Gonz�lez \n17118", background = 'gold3', font=('BolsterBold', 16)).place(x=100,y=10)
title =Label(ventana, text="Contador", background = 'gold3', font=('BolsterBold', 16)).place(x=185,y=80)
answer = Entry(ventana)
answer.place(x=100, y=130)
pot1 = Label(ventana, text="Potenci�metro 1", background = 'gold3', font=('BolsterBold', 16)).place(x=10,y=170)
pot2 = Label(ventana, text="Potenci�metro 2", background = 'gold3', font=('BolsterBold', 16)).place(x=300,y=170)


#Configuraci�n del bot�n
def sendCount():
    ser.flushInput()
    ser.flushOutput()
    time.sleep(.05)
    answer_write = answer.get()
    answer_send = chr(int(answer_write))
    ser.write(answer_send) 
    
send = Button(ventana, text = "Enviar", command=sendCount)
send.place(x=315,y=130)

#Configraci�n de casillas de informaci�n
POT_1 = Label(ventana,text=str(volt_POT1) ,fg="gray18",font=(18))
POT_1.place(x=110,y=230)

POT_2 = Label(ventana,text=str(volt_POT2) ,fg="gray18",font=(18))
POT_2.place(x=315,y=230)

POT_1_1 = Label(ventana,text="V" ,fg="gray18",font=(18))
POT_1_1.place(x=175,y=230)

POT_2_1 = Label(ventana,text="V" ,fg="gray18",font=(18))
POT_2_1.place(x=380,y=230)

#While loop en el que se mostraran los datos de los potenciometros enviados del PIC en la computadora
while True:
    dato = ser.read()
    try:
        if dato == '':
            pass
        else:

            dato_POT1 = ser.read()
            ser.flushOutput()
            dato_POT2 = ser.read()

            real_POT1 = ord(dato_POT1)
            real_POT2 = ord(dato_POT2)

            print(str(real_POT1) + ',' + str(real_POT2))
            ser.flushInput()
            time.sleep(.05)
            
    except:
        print('#')
    volt_POT1 = float((real_POT1)/float (51))
    volt_POT2 = float((real_POT2)/float (51))
    POT_1.config(text = str(volt_POT1)[0:4])
    POT_2.config(text = str(volt_POT2)[0:4])
    ventana.update_idletasks()
    ventana.update() 
    time.sleep(.2)

ventana.mainloop()
