import threading
from threading import Semaphore
import time
import os

tl = 5
tr = 0
tc = 0
idsol = 0
sem = Semaphore(1)


def solicitud_compra(id):
    global tl
    global tr
    print("Hijo nro ", id, "; Padre ", os.getpid())
    time.sleep(1)
    sem.acquire()
    if tl > 0:
        tl -= 1
        tr += 1
        print("Su reserva se ha realizado con exito")
    else:
        print("No hay tickets disponibles")
    time.sleep(1)
    sem.release()


def confirmacion_compra(id):
    global tr
    global tc
    print("Hijo nro ", id, "; Padre ", os.getpid())
    time.sleep(1)
    sem.acquire()
    if tr > 0:
        tr -= 1
        tc += 1
        print("Su compra se ha realizado con exito")
    else:
        print("No hay tickets disponibles")
    time.sleep(1)
    sem.release()


def menu(idsol):
    reply = 1
    print("Reservar Ticket 	- 1")
    print("Comprar Ticket 	- 2")
    print("Salir 			- 3")
    print("Ingrese la opcion deseada: ")
    while reply != 3:
        reply = int(input())
        if reply == 1:
            t = threading.Thread(target=solicitud_compra, args=[idsol+1])
            t.start()
        elif reply == 2:
            t = threading.Thread(target=confirmacion_compra, args=[idsol+1])
            t.start()
        elif reply == 3:
            break
        else:
            print("Comando incorrecto")


menu(idsol)
exit()
