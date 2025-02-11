import json
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd

# Load JSON data
with open('shracunri_live_data_1733273840588.json') as file:
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
        continue

# Create a DataFrame
data = pd.DataFrame({
    "Date": [ts.split("T")[0] for ts in timestamps],
    "Time": [ts.split("T")[1].split("Z")[0] for ts in timestamps],
    "Latitude": [loc[0] for loc in locations],
    "Longitude": [loc[1] for loc in locations],
    "State": states
})

# Initialize the plot
fig, axes = plt.subplots(2, 1, figsize=(10, 8))
table_ax, map_ax = axes
table_ax.axis('off')  # Turn off axis for the table
map_ax.set_title("Device Location")
map_ax.set_xlabel("Longitude")
map_ax.set_ylabel("Latitude")

scatter = map_ax.scatter([], [], c='blue', label="Location")
map_ax.legend()

# Update function for animation
def update(frame):
    table_ax.clear()
    table_ax.axis('off')

    # Display the current row in a table format
    current_row = data.iloc[frame:frame + 1]
    table = table_ax.table(cellText=current_row.values,
                           colLabels=current_row.columns,
                           loc='center',
                           cellLoc='center')
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.5, 1.5)

    # Update scatter plot
    scatter.set_offsets([[data.iloc[frame]["Longitude"], data.iloc[frame]["Latitude"]]])
    map_ax.set_xlim(data["Longitude"].min() - 0.01, data["Longitude"].max() + 0.01)
    map_ax.set_ylim(data["Latitude"].min() - 0.01, data["Latitude"].max() + 0.01)

# Animate
ani = FuncAnimation(fig, update, frames=len(data), interval=1000, repeat=True)

plt.tight_layout()
plt.show()
