
# New Media Firmware [Working Title]

![Emergence logo](https://cdn.kentaa.nl/carousel/file/project_photo/51826/normal_82ba972c625dc7ee9e991e05374556273c52e8d2.png)

<br>
This repository contains the firmware for the hardware modules of the Emergence 2 New Media Project.

---

```markdown

# Prerequisites

## Installations and Setup

### Visual Studio Code
- Download and install Visual Studio Code from the [official website](https://code.visualstudio.com/).

### Node.js and npm
- Follow the instructions to download and install Node.js and npm [here](https://docs.npmjs.com/downloading-and-installing-node-js-and-npm).
- During setup, choose the extra-long installation.
- Restart your computer after installation.
- Verify installation by running the following commands in the terminal:
  ```bash
  node -v
  npm -v
```

### PlatformIO Extension for Visual Studio Code 

- Install the PlatformIO extension from the Visual Studio Code extensions marketplace.

### PostgreSQL 
 
- Download and install PostgreSQL from the [official website](https://www.postgresql.org/) .
 
- Create a new login/group role with the following credentials using pgAdmin (installed with PostgreSQL): 
  - **Username:**  nmpr
 
  - **Password:**  admin
 
- Create a new table called `new_media`.

## GitHub Repositories 
 
- Clone the following repositories from GitHub: 
  - Server code: [server-new-media-em2](https://github.com/EmergenceDelft/server-new-media-em2)
 
  - Firmware code: [firmware-new-media-em2](https://github.com/EmergenceDelft/firmware-new-media-em2)


---


## Connecting and Setting Up ESP 
**Steps 1 to 5 can be skipped if you have the server running on the Raspberry Pi.**  The Raspberry Pi is programmed to start up a Wi-Fi hotspot (NMPRouter) and start the server using `docker-compose up`. 
- **To connect Raspberry Pi to LAN and have internet access, pull from the server repo/develop:**  
  - Connect to NMPRouter
 
  - `ssh nmprouter@192.168.12.1` (password admin)
 
  - **Check if the server is running:** 

```bash
docker ps
```
 
  - **To run the server:** 

```bash
docker-compose up --build
```
 
  - **If any error appears, first run:** 

```bash
docker-compose down
```
 
    - Especially if the error is: `ERROR: for nodeapp 'ContainerConfig'`
 
  - The current branch is `develop`. Run `git pull` to refresh.
 
  - **If the server is running on the Raspberry Pi, the IP address will be:**  `ws://192.168.12.1:5050/`
 
1. **Connect ESP to Power**  
  - Use the ESP labeled B6 with the black cable. Alternatively, flash a new ESP with the code from `scripts/nmproutes_esp.ino`.
 
2. **Connect to NMPRouter Network**  
  - **Network:**  NMPRouter
 
  - **Password:**  keepitquantum
 
  - **Note:**  You will not have internet access after this step. To reconnect to the internet, disconnect from Wi-Fi and connect back to eduroam. But if you want to reconnect to NMPRouter, you will have to check the IP address and restart the server.
 
3. **Server Setup** 
  - Open the server code in Visual Studio Code.
 
  - Open the terminal and run:

```bash
npm install
npm start
```
 
4. **Find Your IP Address**  
  - Run `ipconfig` in the terminal on Windows.
 
  - Note the IP address next to `IPv4`.
 
5. **Firmware Setup** 
  - Open the firmware code in Visual Studio Code.
 
  - In `include/main.h`, set the IP address to your laptop’s IP address, ensuring the format is `ws://<your-ip-address>:5050/echo`.
 
  - **Note:**  This will reset after about 20 minutes and needs to be updated again.
 
6. **Connect to ESP via USB** 
  - Use a good-quality micro-USB cable, one that is good for data transfer, not just charging.

  - Find and set the port to the one labeled USB Enhanced Serial. PlatformIO should have an auto option that should also work.
 
7. **Upload Firmware** 
  - Press the upload button (right arrow) in PlatformIO.

  - Press the boot button on the ESP for a second or two when it says "Connecting...".
 
8. **Optional: Use Serial Monitor** 
  - Press the serial monitor button (looks like a plug) in PlatformIO.

  - Press the EN button on the ESP.


```Copy code
Copy code
```
