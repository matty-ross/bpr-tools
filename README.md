# BPR Tools
A collection of tools for Burnout Paradise Remastered to aid with reverse engineering.

## Proxy Server
A tool that sends an unencrypted network traffic to a local proxy server, so you can analyze it in Wireshark.
### Usage:
Start the `Server.py` script. It will create a local proxy server.
Inject the `Client.dll` into the game. It will automatically connect to the local server and start forwarding the traffic.

## Game Events
A tool that monitors the game events and sends them to a local server for analysis.
### Usage:
Start the `Anlyzer.py` script. It will create a local server where the data will be send to.
Inject the `Dispatcher.dll` into the game. It will send all the data to the server.
