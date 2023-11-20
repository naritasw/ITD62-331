import paho.mqtt.client as mqtt
import json
import requests
import socket

# Initialize a dictionary to store the transformed data
id_counter = 1

def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # Doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        local_ip = s.getsockname()[0]
    except Exception:
        local_ip = '127.0.0.1'
    finally:
        s.close()
    return local_ip

def on_connect(client, userdata, flags, rc):
    print("Connected to broker " + str(rc))
    client.subscribe("DHT11")

def on_message(client, userdata, msg):
    global id_counter
    print(f"message on topic {msg.topic}: {msg.payload.decode()}")

    # Parse the received payload
    payload = msg.payload.decode()

    # Check if the payload contains a failure message
    if "Failed to read from DHT sensor!" in payload:
        handle_sensor_failure()
        return

    id_counter += 1
    # Splitting the payload to extract temperature and humidity
    send_to_json_server(payload)


def send_to_json_server(payload):

    # Create the final payload for the server
    data = json.loads(payload)
    final_data = {}
    final_data["id"] = id_counter
    final_data["temperature"] = data["temperature"]
    final_data["humidity"] = data["humidity"]
    final_data["SensorFailure"] = "False"
    final_data["IP"] = get_local_ip()

    url = "http://localhost:3000/data"  # Change the URL accordingly

    # POST request to the JSON server
    headers = {'Content-type': 'application/json'}
    response = requests.post(url, json=final_data, headers=headers)

    # Reset transformed_data after sending
    transformed_data = {}

client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.137.1", port=1883)

client.loop_forever()