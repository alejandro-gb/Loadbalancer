import sys
import os
import json
import requests
from random import seed
from random import randint

#Limpiar la consola
os.system("cls")

#Variables
ruta = ""
texto = ""
oraciones = []
nombres = []
comentarios = []
direccion = ""
seed(1)


#Clase que define un comentario
class comentario:
    #Constructor, recibe el usuario y el comentario
    def __init__(self, user, comment):
        self.user = user
        self.comment = comment

#Funcion para imprimir las opciones del menu
def menu():
    print("\u001b[32m")
    print("1. Ingresar ruta.")
    print("2. Ingresar direccion.")
    print("3. Ver datos")
    print("4. Enviar datos")
    print("5. Salir")
    print()

#Funcion para obtener la opcion ingresada por el usuario
def getopcion():
    num = 0
    entry = input("Ingresa un opcion:")
    try:
        num = int(entry)
        return num
    except:
        print("Opcion invalida!")

#Ejecutar el menu
menu()
#Obtener la opcion del usuario
opcion = getopcion()

#Mientras la opcion no sea 5 repetir el menu
while opcion != 5:
    #------------------------------------------------INGRESAR RUTA DEL ARCHIVO QUE CONTIENE LAS ORACIONES
    if opcion == 1:
        ruta = input("Ingresa la ruta del archivo:")
        #ruta = "C:\\Users\\Alejandro Garcia\\Desktop\\PROYECTO\\oraciones.txt"
        try:
            assert os.path.exists(ruta), "Ruta incorrecta o el archivo no existe!"
            f = open(ruta, "r+")
            texto = f.read()
            f.close()
        except:
            print("\u001b[31m")
            print("Ingresa una ruta correcta!")
        if len(nombres) == 0:
            assert os.path.exists("C:\\Users\\Alejandro Garcia\\Desktop\\PROYECTO\\CLIENTE\\nombres.txt"), "Hubo un problema al leer los nombres"
            f = open("C:\\Users\\Alejandro Garcia\\Desktop\\PROYECTO\\CLIENTE\\nombres.txt", "r+")
            lista = f.read()
            nombres = lista.split("\n")
            f.close()
    #--------------------------------------------------INGRESAR LA DIRECCION IP DEL SERVIDOR
    elif opcion == 2:
        print("\u001b[34m")
        direccion = input("Ingresa la direccion web -> ")
    #--------------------------------------------------VER LOS DATOS DEL ARCHIVO
    elif opcion == 3:
        print("\u001b[37m")
        oraciones = texto.split(".")
        print("Hay " + str(len(oraciones)) + " comentarios")
        for i in range(0,len(oraciones)-1):
            value = randint(0,len(nombres)-1)
            newcomentario = comentario(nombres[value],oraciones[i])
            comentarios.append(newcomentario)
        
        for c in comentarios:
            print(c.user + " -> " + c.comment)
    #----------------------------------------------------ENVIAR LOS DATOS AL SERVIDOR
    elif opcion == 4:
        print("\u001b[36m")
        print("Enviando los datos a " + direccion)
        
        for c in comentarios:
            uname = c.user
            ucom = c.comment
            data = {
                'user' : uname,
                'comentario' : ucom
            }
            post = requests.post(direccion, json=data)
            print((post.json())['resultado'])
            
    #------------------------------------------------------------OPCION INVALIDA
    else:
        print("\u001b[33m")
        print("opcion invalida!")
    
    #Ejecutar el menu
    menu()
    #Obtener la opcion del usuario
    opcion = getopcion()





