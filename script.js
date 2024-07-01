

document.addEventListener('DOMContentLoaded', () => {



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








    document.getElementById('clickable-button').addEventListener('click', () => {
        console.log('Clickable button clicked');
        ws.send(JSON.stringify({action:'clickableButton', data:100}));
    });

    document.getElementById('clickable-link').addEventListener('click', (event) => {
        event.preventDefault();
        console.log('Clickable link clicked');
        ws.send(JSON.stringify({action:'clickableLink', data:100}));
    });

    document.getElementById('range').addEventListener('input', (event) => {
        console.log(`Slider value: ${event.target.value}`);
        ws.send(JSON.stringify({action:'slider', data:event.target.value}));
    });

// myModal functionality
    const myModal = document.getElementById("modal");
    const myModalTrigger = document.getElementById("modal-trigger");
    const closeButton = document.getElementsByClassName("close-button")[0];

    myModalTrigger.onclick = function () {
        myModal.style.display = "block";
        ws.send(JSON.stringify({action:'modalOpens', data:100}));

    }

    closeButton.onclick = function () {
        myModal.style.display = "none";
        ws.send(JSON.stringify({action:'modalCloses', data:100}));

    }

    window.onclick = function (event) {
        if (event.target == myModal) {
            myModal.style.display = "none";
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


    //Non-clickable Elements
    const nonClickableDiv = document.getElementById("non-clickable-div");
    const nonClickableParagraph = document.getElementById("non-clickable-paragraph");
    nonClickableDiv.addEventListener('mouseover',(event) => {
        ws.send(JSON.stringify({action:'hoverNonClickable', data:100}));
    });
    nonClickableDiv.addEventListener('click',(event) => {
        console.log("click")
        ws.send(JSON.stringify({action:'clickNonClickable', data:100}));
    })
    nonClickableParagraph.addEventListener('mouseover',(event) => {
        ws.send(JSON.stringify({action:'hoverNonClickable', data:100}));
    });
    nonClickableParagraph.addEventListener('click',(event) => {
        ws.send(JSON.stringify({action:'clickNonClickable', data:100}));
    });


    //Form-elements

});