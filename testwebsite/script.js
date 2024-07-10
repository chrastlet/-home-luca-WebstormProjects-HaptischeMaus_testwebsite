document.addEventListener('DOMContentLoaded', () => {
    let port;
    let writer;
    let textInputInterval;

    const ws = new WebSocket('ws://192.168.137.212/ws');
    const statusDisplay = document.getElementById('status');
    const sendButton = document.getElementById('sendButton');

    ws.onopen= function(){
        console.log("Websocket connected");
    }
    ws.onmessage = function(event) {
        console.log('Message from server:', event.data);
    };

    ws.onclose = function() {
        statusDisplay.innerText = 'Disconnected';
        console.log('WebSocket disconnected');
    };

    ws.onerror = function(error) {
        console.error('WebSocket error:', error);
    };



    const sendToArduino = async (message, data) => {
            try {
                ws.send({
                    message: message,
                    data: data
                })
                console.log('Message sent to Arduino:', message);
            } catch (e) {
                console.error('Error writing to serial port:', e);
            }
        }



    // Drag and drop functionality
    const draggable = document.getElementById('draggable');
    const dropZone = document.getElementById('drop-zone');

    draggable.addEventListener('dragstart', (event) => {
        event.dataTransfer.setData('text', event.target.id);
        console.log('Dragging started');
        ws.send(JSON.stringify({action:'startDrag', data:100}));
    });

    dropZone.addEventListener('dragover', (event) => {
        event.preventDefault();
        event.dataTransfer.dropEffect = 'move';

        // Berechne die Mauskoordinaten relativ zur dropZone
        const rect = dropZone.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;

        // Berechne die Prozentwerte
        const xPercent = (x / rect.width) * 100;
        const yPercent = (y / rect.height) * 100;

        ws.send(JSON.stringify({action:'startDrag', data:[xPercent, yPercent]}));

    });

    dropZone.addEventListener('drop', (event) => {
        event.preventDefault();
        const data = event.dataTransfer.getData('text');
        const draggableElement = document.getElementById(data);
        dropZone.appendChild(draggableElement);
        console.log('Dropped in drop zone');
        ws.send(JSON.stringify({action:'drop', data:100}));

    });


    document.getElementById('clickable-button').addEventListener('click', () => {
        console.log('Clickable button clicked');
        sendToArduino('button',100);
    });

    document.getElementById('clickable-link').addEventListener('click', (event) => {
        event.preventDefault();
        console.log('Clickable link clicked');
        sendToArduino('link', 100);
    });

    document.getElementById('text-input').addEventListener('focus', () => {
        textInputInterval = setInterval(() => {
            sendToArduino('text-input', 100);
        }, 1000); // Continuously send 'F' command every second while focused
    });

    document.getElementById('text-input').addEventListener('blur', () => {
        clearInterval(textInputInterval); // Stop sending 'F' command when focus is lost
    });

    document.getElementById('textarea').addEventListener('focus', () => {
        textInputInterval = setInterval(() => {
            sendToArduino('textarea',100);
        }, 1000); // Continuously send 'F' command every second while focused
    });

    document.getElementById('textarea').addEventListener('blur', () => {
        clearInterval(textInputInterval); // Stop sending 'F' command when focus is lost
    });

    document.getElementById('checkbox').addEventListener('change', () => {
        sendToArduino('checkbox', 100);
    });

    document.getElementById('radio').addEventListener('change', () => {
        sendToArduino('radio', 100);
    });


    document.getElementById('select').addEventListener('click', () => {
        sendToArduino('select_click', 100);
    });

    document.getElementById('select').addEventListener('change', (event) => {
        const value = event.target.value;
        sendToArduino('select_change',100);
    });

    document.getElementById('range').addEventListener('input', (event) => {
        console.log(`Slider value: ${event.target.value}`);
        sendToArduino('range', `${event.target.value}`);
    });

    document.querySelector('form').addEventListener('submit', (event) => {
        event.preventDefault();
        sendToArduino('submit', 100);
    });


    const scrollableArea = document.getElementById('scrollable-area');
    scrollableArea.addEventListener('scroll', () => {
        if (scrollableArea.scrollTop + scrollableArea.clientHeight >= scrollableArea.scrollHeight || scrollableArea.scrollTop === 0) {
            sendToArduino('E'); // End of vertical scroll
        } else if (scrollableArea.scrollLeft + scrollableArea.clientWidth >= scrollableArea.scrollWidth || scrollableArea.scrollLeft === 0) {
            sendToArduino('E'); // End of horizontal scroll
        }
    });

    // Add any additional event listeners as needed
});
