const SERVER_ADDRESS = 'ws://192.168.0.12:8081/live';

console.log('Trying to connect to server at', SERVER_ADDRESS);

const socket = new WebSocket(SERVER_ADDRESS);

// Connection opened
socket.addEventListener('open', function (event) {
    console.log('[socket opened]');

    socket.send('Hello Server');
});

// Listen for messages
socket.addEventListener('message', function (event) {
    console.log('[socket message]');

    console.log('Message from server: ', event.data);
});
