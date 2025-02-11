# Smart-Bicycle-Theft-Prevention-System
An IoT-based bicycle security solution integrating NFC authentication, LoRa communication, and a web interface to provide real-time alerts and remote monitoring against theft.

The Smart Bicycle Theft Prevention System integrates NFC and LoRaWAN technologies to provide robust security and real-time monitoring for bicycles. NFC allows authorized users to lock or unlock the bicycle and update its state with date, time, and location, while LoRaWAN enables long-range, low-power communication to transmit periodic location updates to The Things Network (TTN). The system sends the bicycle's location every 15 minutes when locked, and if unauthorized movement is detected, an alert is triggered, increasing the update frequency to every 2 minutes for real-time tracking. The alert mode can be reset by scanning the NFC module. This innovative solution combines theft prevention, remote tracking, and energy efficiency, ensuring enhanced bicycle security and user convenience.

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

Execution Process:

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

