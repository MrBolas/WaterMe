FROM node:10

# Create Main app directory
WORKDIR /usr/src/WaterMe

# Create Sub Apps Directory -> 
WORKDIR /usr/src/WaterMe/Software/MQTTClient

# Install app dependencies
COPY Software/MQTTClient/package*.json ./

RUN npm install

# Bundle app source
COPY Software/MQTTClient/ .

EXPOSE 1883

CMD [ "npm", "run", "start_mqtt_client" ]
