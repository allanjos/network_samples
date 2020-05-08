# MQTT communication sample

## Broker

Build and install MQTT broker:

    sudo apt install uthash-dev
    wget -c https://mosquitto.org/files/source/mosquitto-1.6.9.tar.gz
    tar xfz mosquitto-1.6.9.tar.gz
    cd mosquitto-1.6.9/
    cmake .
    make
    sudo make install

Start MQTT broker:

    mosquitto -d

## Clients

Execute the first client:

    python3 client1.py

Execute the second client:

    python3 client2.py
