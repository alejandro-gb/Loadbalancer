from flask import Flask, jsonify, request, Response
from flask_cors import CORS
import json
import socket
import requests

hostname = socket.gethostname()
ipAddress = socket.gethostbyname(hostname)
SA = 'http://3.138.52.145:3000'
SB = 'http://3.131.112.199:3000'

app = Flask(__name__)
CORS(app)

@app.route('/saludo',methods = ['GET'])
def Saludar():
    resultado = jsonify({
        'servidor':'Hola desde Servidor1',
        'hostname':hostname,
        'ip':ipAddress
    })
    return resultado

def enviar(val,data):
    print(val)
    print(data)
    if val:
        r = requests.post(url = SA + "/publicar", json = data)
        print(r.text)
    else:
        r = requests.post(url = SB + "/publicar", json = data)
        print(r.text)


@app.route('/recibir', methods = ['POST'])
def recibirDatos():
    usuario = request.json['user']
    coment = request.json['comentario']
    if usuario and coment:
        data = {
            'user':usuario,
            'comentario':coment
        }
        resultado = "ninguno"
        
        ra = requests.get(SA+"/count")
        numa = ra.json()
        rb = requests.get(SB+"/count")
        numb = rb.json()
        
        ramA = requests.get(SA + "/ram")
        numRamA = ramA.json()
        ramB = requests.get(SB + "/ram")
        numRamB = ramB.json()

        #cpua = requests.get(SA + "/cpu")
        #numcpua = cpua.json()
        #cpub = requests.get(SB + "/cpu")
        #numcpub = cpub.json()
        
        servidor = True;
        if(numa > numb):
            servidor = False
            resultado = "enviado a B cuenta"
        elif (numb > numa):
            resultado = "enviado a A cuenta"
        elif (numRamA['porcentaje'] > numRamB['porcentaje']):
            servidor = False
            resultado = "enviado a B RAM"
        elif (numRamA['porcentaje'] < numRamB['porcentaje']):
            resultado = "enviado a A RAM"
        #elif (numcpua > numcpub):
        #    resultado = "enviado a B CPU"
        #elif (numcpua < numcpub):
        #    resultado = "enviado a A CPU"
        else:
            resultado = "Enviado a A DEFAULT"
        enviar(servidor,data)
        response = jsonify({
            'result':'listo',
            'countA':numa,
            'countB':numb,
            'ramA':numRamA['porcentaje'],
            'ramB':numRamB['porcentaje'],
            #'cpuA':numcpua,
            #'cpuB':numcpub,
            'resultado':resultado
        })
        response.status_code = 201
        return response
    else:
        return not_found()

if __name__ == "__main__":
    app.run(host = "0.0.0.0",port=80, debug= True)