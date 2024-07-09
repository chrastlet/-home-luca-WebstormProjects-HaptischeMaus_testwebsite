document.addEventListener('DOMContentLoaded', () => {
    let port;
    let writer;
    let textInputInterval;

    const connectButton = document.getElementById('connect-arduino');

    connectButton.addEventListener('click', async () => {
        try {
            port = await navigator.serial.requestPort();
            console.log('Serial port requested');
            await port.open({ baudRate: 9600 });
            console.log('Serial port opened');

            writer = port.writable.getWriter();
            console.log('Writer created');

            console.log('Connected to Arduino');
        } catch (e) {
            console.error('Failed to connect to Arduino:', e.message);
            alert('Failed to connect to Arduino. Please ensure the port is correct and you have permissions.');
        }
    });

    const sendToArduino = async (message) => {
        if (writer) {
            try {
                await writer.write(new TextEncoder().encode(message + '\n'));
                console.log('Message sent to Arduino:', message);
            } catch (e) {
                console.error('Error writing to serial port:', e);
            }
        } else {
            console.warn('Writer is not available');
        }
    };

    document.getElementById('clickable-button').addEventListener('click', () => {
        console.log('Clickable button clicked');
        sendToArduino('C');
    });

    document.getElementById('clickable-link').addEventListener('click', (event) => {
        event.preventDefault();
        console.log('Clickable link clicked');
        sendToArduino('L');
    });

    document.getElementById('text-input').addEventListener('focus', () => {
        textInputInterval = setInterval(() => {
            sendToArduino('F');
        }, 1000); // Continuously send 'F' command every second while focused
    });

    document.getElementById('text-input').addEventListener('blur', () => {
        clearInterval(textInputInterval); // Stop sending 'F' command when focus is lost
    });

    document.getElementById('textarea').addEventListener('focus', () => {
        textInputInterval = setInterval(() => {
            sendToArduino('F');
        }, 1000); // Continuously send 'F' command every second while focused
    });

    document.getElementById('textarea').addEventListener('blur', () => {
        clearInterval(textInputInterval); // Stop sending 'F' command when focus is lost
    });

    document.getElementById('checkbox').addEventListener('change', () => {
        sendToArduino('B');
    });

    document.getElementById('radio').addEventListener('change', () => {
        sendToArduino('R');
    });

    document.getElementById('select').addEventListener('click', () => {
        sendToArduino('D');
    });

    document.getElementById('select').addEventListener('change', (event) => {
        const value = event.target.value;
        sendToArduino(`S${value}`);
    });

    document.getElementById('range').addEventListener('input', (event) => {
        console.log(`Slider value: ${event.target.value}`);
        sendToArduino(`S${event.target.value}`);
    });

    document.querySelector('form').addEventListener('submit', (event) => {
        event.preventDefault();
        sendToArduino('U');
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
