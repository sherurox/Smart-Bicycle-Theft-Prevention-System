# Smart-Bicycle-Theft-Prevention-System
An IoT-based bicycle security solution integrating NFC authentication, LoRa communication, and a web interface to provide real-time alerts and remote monitoring against theft.
<img width="230" alt="image" src="https://github.com/user-attachments/assets/c9217a56-72a9-4413-adff-c765ca1ea3e6" />

The Smart Bicycle Theft Prevention System integrates NFC and LoRaWAN technologies to provide robust security and real-time monitoring for bicycles. NFC allows authorized users to lock or unlock the bicycle and update its state with date, time, and location, while LoRaWAN enables long-range, low-power communication to transmit periodic location updates to The Things Network (TTN). The system sends the bicycle's location every 15 minutes when locked, and if unauthorized movement is detected, an alert is triggered, increasing the update frequency to every 2 minutes for real-time tracking. The alert mode can be reset by scanning the NFC module. This innovative solution combines theft prevention, remote tracking, and energy efficiency, ensuring enhanced bicycle security and user convenience.

#Problems Tackled:
Challenges in Existing Bicycle Sharing Systems
Infrastructure Costs: Requires expensive, dedicated docking stations.
Limited Flexibility: Restricts users to fixed pick-up and drop-off locations.
Operational Expenses: High costs for battery recharging and managing fleets.

My Solution
Infrastructure-Free: Eliminates the need for costly docking stations.
Energy-Efficient: Powered by low-energy technology.
Cost-Effective: Reduces operational and maintenance costs.

#Enhanced Bicycle Theft Prevention System

Periodic Location Updates:
Once the bike is locked, its location is sent periodically to The Things Network (TTN) and displayed on the website every 15 minutes.
Alert Activation:
If the bike’s location (latitude and longitude) changes while in the "LOCKED" state, an immediate ALERT is triggered.
High-Frequency Tracking:
In "ALERT" mode, the location update frequency increases from every 15 minutes to every 2 minutes to enable real-time tracking of the bicycle.
Deactivating Alert Mode:
To transition from "ALERT" mode back to "UNLOCK" mode, the user simply scans the NFC module on the bicycle.

#Components and Data Flow
<img width="1087" alt="Screenshot 2025-02-11 at 1 06 37 PM" src="https://github.com/user-attachments/assets/07519a72-f32a-43f3-8b83-166eb39ac71e" />

Key Components
1.	NFC Technology:
NFC acts as a user authentication interface. The NFC tag or module embedded in the bicycle allows authorized users to interact with the system by scanning it with an NFC-enabled device. This triggers state changes, such as locking or unlocking the bicycle, and updates the system with relevant metadata like date, time, and location.
2.	LoRaWAN Communication:
LoRaWAN enables long-range, low-power communication between the bicycle and a central server or cloud platform like The Things Network (TTN). It facilitates periodic location updates and alert notifications to the user or monitoring system.
3.	Geolocation and State Monitoring:
The system uses fixed or dynamically updated geolocation data to monitor the bicycle's location. Changes in state, such as "LOCKED" or "UNLOCKED," are tracked and reported, ensuring that the current status of the bicycle is always known.
4.	Security Features:
o	Periodic Location Updates: When the bicycle is locked, the system sends its location to TTN every 15 minutes, providing consistent tracking data.
o	Alert Activation: If the bicycle moves from its locked location, the system triggers an alert and increases the frequency of location updates to every 2 minutes, allowing real-time tracking.
o	Deactivating Alerts: The alert mode is reset by scanning the NFC module again, which transitions the bicycle back to the unlocked state.
Applications and Benefits
•	Theft Prevention: By combining NFC-based authentication and LoRaWAN-enabled monitoring, the system ensures that only authorized users can interact with the bicycle. Alerts for unauthorized movements further enhance security.
•	Remote Tracking: Long-range communication via LoRaWAN allows users to monitor their bicycle's location from a centralized platform, even when it is far from their immediate vicinity.
•	Low Power Consumption: The use of LoRaWAN ensures energy efficiency, enabling the system to operate for extended periods on minimal power.
•	User Convenience: The NFC module simplifies user interaction, making it easy to lock, unlock, or reset the system with a simple scan.

#Hardware Connection: LoRa and NFC Devices

Heltec WiFi LoRa 32 V3 (LoRa Device):
TX (Transmit): Transmits data from the LoRa device.
RX (Receive): Receives data on the LoRa device.
GND (Ground): Shared ground connection with the NFC device.
nRF52840 (NFC Device):
TX (Transmit): Transmits data from the NFC device.
RX (Receive): Receives data on the NFC device.
GND (Ground): Shared ground connection with the LoRa device.

UART (Universal Asynchronous Receiver-Transmitter): Enables seamless data exchange between the LoRa and NFC devices through their TX and RX pins, with GND ensuring a stable connection.

<img width="828" alt="image" src="https://github.com/user-attachments/assets/611d2122-2da7-475a-b5eb-f32efb656db3" />

<img width="828" alt="image" src="https://github.com/user-attachments/assets/c7046dcf-025c-4172-b88e-eb9a27ee6bf7" />

<img width="1102" alt="Screenshot 2025-02-11 at 1 08 18 PM" src="https://github.com/user-attachments/assets/8a6bf127-6b8b-4670-a4d7-8a8733650ced" />


#Execution Process:

PART1 NFC SCAN:

The first step is to change the bike, i.e from LOCK to UNLOCK and vice versa, msg sent from nrf device to the esp LoRa device
Open, build and flash the NFC_SCAN.c into the nrf board 

PART 2 LoRa Payload Upload:
This code LoRa.cpp was supposed to take payload (STATE: LOCKED, Location: 42.089654, -75.970346) from the NFC nrf device to LoRa esp device and upload it to the TTN, but we were unable to establish successful connection between the nrf and esp device, and so for testing purpose we developed another code LoRa_test.cpp which sent the payload (STATE: LOCKED, Location: 42.089654, -75.970346) to the ttn.
Open, build and flash the LoRa_test.cpp into the esp board, make sure the antenna is connected to esp board 

Use the update.js to update the formatting in Payload Formatters
PART 3 Website
Right-click on the folder, select "Send to" > "Compressed (zipped) folder," and rename the resulting zip file (e.g., smart_bike_webserver.zip)
To load and execute the project , start by ensuring the folder structure is correctly organized. The smart_bike_webserver folder should contain subfolders static (which includes files like script.js, style.css, and image assets such as Lock.gif, 22.jpg, and 2.gif) and templates (which contains the main index.html file). Alongside these, the root folder must include the Python file app.py, the live_data.json file for storing data, and the requirements.txt file for installing dependencies.
Once the structure is confirmed, open a terminal and navigate to the smart_bike_webserver directory. Use the command pip install -r requirements.txt to install all the necessary Python libraries listed in the requirements.txt file. After the dependencies are installed, you can start the web server by running the command python app.py. This will initialize the server locally, typically accessible at http://127.0.0.1:5000. Open this URL in a web browser to verify that the website is loading correctly and fetching data from the live_data.json file as expected

#Challenges Faced and Solution Implemented

The connection between the NFC and LoRa devices could not be successfully established due to issues in UART implementation.
Temporary Solution:
A new LoRa code was developed to simulate the location data.
Fixed Location Values:
Latitude: 42.089654
Longitude: -75.970346 (Binghamton University).
Outcome:
This simulated data was sent to The Things Network (TTN) and successfully extracted to the website for testing purposes.

#Future enhancements

Time Update Optimization:
      Ensure real-time to improve responsiveness in alerting users.
Enhanced Communication:
      Improve the connection and data transfer efficiency between the NFC and LoRa modules for seamless operation.
Unique User Authentication:
      Implement a robust authentication system to ensure only authorized users can access the bike, increasing security.
Integration of GNSS Module:
     Add a Global Navigation Satellite System (GNSS) module to the NFC device for precise location tracking.
     Enable the NFC module to send exact latitude and longitude coordinates to the LoRa module for accurate theft tracking    
     we used fix location coordinate values for our testing.
Alert System Testing:
     Regularly test the alert system to ensure it functions reliably in various scenarios as we were unable to check it in a real-time                 
     scenario.
     Fine-tune alert thresholds to minimize false positives and maximize response time.

