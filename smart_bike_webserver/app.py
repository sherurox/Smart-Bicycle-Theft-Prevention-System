from flask import Flask, render_template, jsonify
import json
import os
import pandas as pd

app = Flask(__name__)

# Load and process data
file_path = r"C:\Users\Dell\Downloads\Smart Cycle Theft Detection System\smart_bike_webserver\shracunri_live_data_1733273840588.json"
if os.path.exists(file_path):
    with open(file_path, 'r') as file:
        raw_data = json.load(file)

    # Extract required fields
    timestamps = []
    locations = []
    states = []

    for entry in raw_data:
        try:
            uplink_message = entry['data']['uplink_message']
            location = uplink_message['decoded_payload']['location']
            state = uplink_message['decoded_payload']['state']
            timestamp = uplink_message['received_at']

            timestamps.append(timestamp)
            locations.append((location['latitude'], location['longitude']))
            states.append(state)
        except KeyError:
            continue  # Skip entries with missing keys

    # Create a DataFrame
    data = pd.DataFrame({
        "Date": [ts.split("T")[0] for ts in timestamps],
        "Time": [ts.split("T")[1].split("Z")[0] for ts in timestamps],
        "Latitude": [loc[0] for loc in locations],
        "Longitude": [loc[1] for loc in locations],
        "State": states
    })
else:
    data = pd.DataFrame(columns=["Date", "Time", "Latitude", "Longitude", "State"])


@app.route('/')
def index():
    # Render the HTML page with the data
    return render_template('index.html', table_data=data.to_dict(orient="records"))


@app.route('/api/data')
def get_data():
    # Provide JSON data via an API endpoint
    return jsonify(data.to_dict(orient="records"))


if __name__ == "__main__":
    app.run(debug=True)
