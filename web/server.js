const express = require('express');
const http = require('http');
const { Server } = require("socket.io");
const { spawn } = require('child_process');
const path = require('path');
const readline = require('readline');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

const PORT = 3000;

// Serve static files from the 'web' directory
app.use(express.static(path.join(__dirname)));

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'index.html'));
});

function spawnKernel(socket) {
    const kp = spawn(path.join(__dirname, '../bin/medical_kernel'));
    const rl = readline.createInterface({ input: kp.stdout, crlfDelay: Infinity });

    rl.on('line', (line) => {
        if (!line) return;
        try {
            const json = JSON.parse(line);
            socket.emit('kernel_data', json);
        } catch {
            socket.emit('kernel_data', { type: 'log', message: line });
        }
    });
    kp.stderr.on('data', (data) => {
        socket.emit('kernel_data', { type: 'log', message: `ERROR: ${data}` });
    });
    kp.on('close', (code) => {
        socket.emit('kernel_data', { type: 'log', message: `--- KERNEL PROCESS TERMINATED (code ${code}) ---` });
    });
    return kp;
}

io.on('connection', (socket) => {
    console.log('A user connected');
    let kernelProcess = spawnKernel(socket);

    socket.on('stop_stream', () => {
        if (kernelProcess) {
            kernelProcess.kill();
            kernelProcess = null;
        }
    });

    socket.on('start_stream', () => {
        if (!kernelProcess) {
            kernelProcess = spawnKernel(socket);
        }
    });

    socket.on('disconnect', () => {
        if (kernelProcess) {
            kernelProcess.kill();
            kernelProcess = null;
        }
        console.log('User disconnected');
    });
});

server.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`);
    console.log("Open a web browser to this address to see the visualization.");
});
