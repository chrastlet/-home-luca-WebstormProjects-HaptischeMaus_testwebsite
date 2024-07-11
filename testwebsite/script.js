document.addEventListener('DOMContentLoaded', () => {
    let port;
    let writer;
    let textInputInterval;

    const ws = new WebSocket('ws://192.168.114.153/ws');
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
                ws.send(JSON.stringify({
                    message: message,
                    data: data
                }));

                console.log('Message sent to Arduino:', message);
            } catch (e) {
                console.error('Error writing to serial port:', e);
            }
        }

    // Modal functionality
    const myModal = document.getElementById("modal");
    const myModalTrigger = document.getElementById("modal-trigger");
    const closeButton = document.getElementsByClassName("close-button")[0];

    myModalTrigger.onclick = function () {
        myModal.style.display = "block";
        sendToArduino('modalOpens', 100);
    }

    closeButton.onclick = function () {
        myModal.style.display = "none";
        sendToArduino('modalCloses', 100);
    }

    window.onclick = function (event) {
        if (event.target == myModal) {
            myModal.style.display = "none";
        }
    }



    const changeCursorTemporarily = (element, cursorType, message) => {
        const originalCursor = document.body.style.cursor;
        document.body.style.cursor = cursorType;

        const intervalId = setInterval(() => {
            sendToArduino(message, `Cursor changed to ${cursorType}`);
        }, 1000); // Send message every second

        setTimeout(() => {
            clearInterval(intervalId);
            document.body.style.cursor = originalCursor;
        }, 5000); // Reset cursor after 5 seconds
    };

    document.getElementById('wait-cursor-button').addEventListener('click', () => {
        changeCursorTemporarily(document.body, 'wait', 'Wait Cursor');
    });

    document.getElementById('pointer-cursor-button').addEventListener('click', () => {
        changeCursorTemporarily(document.body, 'pointer', 'Pointer Cursor');
    });

    document.getElementById('crosshair-cursor-button').addEventListener('click', () => {
        changeCursorTemporarily(document.body, 'crosshair', 'Crosshair Cursor');
    });

    document.getElementById('text-cursor-button').addEventListener('click', () => {
        changeCursorTemporarily(document.body, 'text', 'Text Cursor');
    });

    document.getElementById('move-cursor-button').addEventListener('click', () => {
        changeCursorTemporarily(document.body, 'move', 'Move Cursor');
    });

    /*
    // Drag and drop functionality
    const draggable = document.getElementById('draggable');
    const dropZone = document.getElementById('drop-zone');

    draggable.addEventListener('dragstart', (event) => {
        event.dataTransfer.setData('text', event.target.id);
        console.log('Dragging started');
        sendToArduino('startDrag', 100);
    });

    dropZone.addEventListener('dragover', (event) => {
        event.preventDefault();
        event.dataTransfer.dropEffect = 'move';

        const rect = dropZone.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const y = event.clientY - rect.top;
        const xPercent = (x / rect.width) * 100;
        const yPercent = (y / rect.height) * 100;

        ws.send(JSON.stringify({action:'dragover', data:[xPercent, yPercent, rect.width, rect.height]}));

    });

    dropZone.addEventListener('drop', (event) => {
        event.preventDefault();
        const data = event.dataTransfer.getData('text');
        const draggableElement = document.getElementById(data);
        dropZone.appendChild(draggableElement);
        console.log('Dropped in drop zone');
        sendToArduino('drop', 100);
    });
*/
    document.getElementById('clickable-button').addEventListener('click', () => {
        console.log('Clickable button clicked');
        sendToArduino('button', 100);
    });

    document.getElementById('clickable-link').addEventListener('click', (event) => {
        event.preventDefault();
        console.log('Clickable link clicked');
        sendToArduino('link', 100);
    });

    document.getElementById('text-input').addEventListener('focus', () => {
        textInputInterval = setInterval(() => {
            sendToArduino('text-input', 100);
        }, 1000);
    });

    document.getElementById('text-input').addEventListener('blur', () => {
        clearInterval(textInputInterval);
    });

    document.getElementById('textarea').addEventListener('focus', () => {
        textInputInterval = setInterval(() => {
            sendToArduino('textarea', 100);
        }, 1000);
    });


    document.getElementById('context-menu-area').addEventListener('click',()=>{
        sendToArduino('context', 100);

    })


    document.getElementById('tooltip-area').addEventListener('click',()=>{
        sendToArduino('tooltip', 100);

    })


    document.getElementById("hover-area").addEventListener("click", () =>{
        sendToArduino('hover', 100);

    })
    document.getElementById('textarea').addEventListener('blur', () => {
        clearInterval(textInputInterval);
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
        sendToArduino('select_change',value);
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
            sendToArduino('endVertical'); // End of vertical scroll
        } else if (scrollableArea.scrollLeft + scrollableArea.clientWidth >= scrollableArea.scrollWidth || scrollableArea.scrollLeft === 0) {
            sendToArduino('endHorizontal'); // End of horizontal scroll
        }
    });


    const zoomArea = document.getElementById('zoomArea');
    let zoomedIn = false;

    zoomArea.addEventListener('click', function() {
        if (zoomedIn) {
            sendToArduino('zoomOut',100);

            zoomArea.classList.remove('zoomed');
            zoomArea.style.backgroundSize = '100% 100%';
        } else {
            sendToArduino('zoomIn',100);

            zoomArea.classList.add('zoomed');
            zoomArea.style.backgroundSize = '200% 200%';
        }

        zoomedIn = !zoomedIn;
    });

    zoomArea.addEventListener('mousemove', function(event) {
        if (zoomedIn) {
            const rect = zoomArea.getBoundingClientRect();
            const x = event.clientX - rect.left;
            const y = event.clientY - rect.top;
            zoomArea.style.backgroundPosition = `-${x}px -${y}px`;
        }
    });


    // Add any additional event listeners as needed
});
