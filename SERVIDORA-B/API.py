from flask import Flask, jsonify, request, Response
from flask_pymongo import PyMongo
from flask_cors import CORS
from bson import json_util
import json

app = Flask(__name__)
CORS(app)
app.config['MONGO_URI'] = 'mongodb://3.138.52.145:27017/local'
mongo = PyMongo(app)

@app.route('/comentarios',methods = ['GET'])
def getComents():
    comentarios = mongo.db.comentarios.find()
    response = json_util.dumps(comentarios)
    return Response(response, mimetype = 'application/json')

@app.route('/publicar', methods = ['POST'])
def newPublicacion():
    usuario = request.json['user']
    coment = request.json['comentario']
    if usuario and coment:
        nuevo = mongo.db.comentarios.insert(
            {'user':usuario,'comentario':coment}
        )
        response = jsonify({
            'result':"listo"
        })
        response.status_code = 201
        return response
    else:
        return not_found()

@app.route('/count', methods=['GET'])
def getCount():
    num = mongo.db.comentarios.count()
    response = json_util.dumps(num)
    return Response(response, mimetype="application/json")

@app.route('/ram', methods=['GET'])
def getRam():
    file = open("/proc/modram", 'r')
    datos = file.read()
    datos = json.loads(datos)
    print(datos)
    return datos

@app.route('/cpu', methods=['GET'])
def getCpu():
    num = 0
    response = json_util.dumps(num)
    return Response(response,mimetype="application/json")


if __name__ == "__main__":
    app.run(host = "0.0.0.0", port=3000, debug=True)