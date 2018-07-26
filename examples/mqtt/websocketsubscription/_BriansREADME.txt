the command line below:

websocketsubscription.exe --host ws://iot.eclipse.org:80/ws --topic platforms/uav/effect3/sensors/gps/lon -p 5

means connect to the eclipse MQTT server, using the the above topic, and publish a message with the payload of "5". To transmit the lat, just change the topic's last item to be "lat" instead of "lon". This is the style used in the SMI ICD.