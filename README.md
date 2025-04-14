`piezoAPI` is used to seamlessly serialize request to the Piezo Pusher. <br>
For detailed information, please look at `piezoAPI.c` for documentation and explanation of the API, as well as the example files for guidance. <br>

<br> `waveform_spi.ino` is an easy to use Arduino program that takes in commands over the serial console, tokenizes, serializes, and sends the data over SPI to the Piezo Pusher. <br>

`processMessage()` is useful if you want to tokenize a typed in request (ex. a message from console) into the corresponding
waveform, amplitude, and frequency. `example.c` demonstrates this. <br> 

`serializeMessage()` takes the waveform characteristics and breaks it down into a packet of 4 bytes and stores it into the buffer
passed in. This buffer should then be transmitted in order with SPI to the Piezo Pusher. See example files. <br> <br>
