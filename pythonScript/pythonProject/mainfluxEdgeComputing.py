import paho.mqtt.client as mqtt
import json
import requests

# MainFlux konfiguracija
mainflux_url = "http://localhost/http/channels/eb329530-eeb9-4e8f-bf57-8c19e47cfe34/messages"
headers = {
    'Content-Type': 'application/json',
    'Authorization': 'Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MTcxODE3OTAsImlhdCI6MTcxNzE0NTc5MCwiaXNzIjoibWFpbmZsdXguYXV0aCIsInN1YiI6InRhbWFyYTFAZ21haWwuY29tIiwiaXNzdWVyX2lkIjoiZmE3YThiMjktNDQxYi00MWZkLWIwYzYtYzcyM2VlMTlhMTNmIiwidHlwZSI6MH0.rdbi_BjC9W-ABYJ85TKIfZIqwHmZul9bmkAcZo1PvqE'
}

def map_value(value, in_min, in_max, out_min, out_max):
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("hello/topic")

def on_message(client, userdata, msg):
    data = json.loads(msg.payload)
    print(f"Received message: {data}")

    data['temperature'] = (data['temperature'] * 9 / 5) + 32  # Konverzija u Fahrenheit
    data['pressure'] = map_value((data['pressure']), 0, 1023, 900, 1100) / 10.0

    response = requests.post(mainflux_url, headers=headers, data=json.dumps(data))
    print(f"Data sent to MainFlux: {response.status_code}")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

mqtt_username = "user1"
mqtt_password = "pass1"
client.username_pw_set(mqtt_username, mqtt_password)

client.connect("localhost", 9001, 6000)
client.loop_forever()
