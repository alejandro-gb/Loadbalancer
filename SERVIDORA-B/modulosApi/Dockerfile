FROM node:12

WORKDIR /usr/src/app

COPY package*.json ./

RUN npm install

COPY . .

EXPOSE 8080

RUN mkdir -p /elements/procs

CMD [ "node", "api.js" ]