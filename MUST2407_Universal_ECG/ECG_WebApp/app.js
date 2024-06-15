const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
// const {SerialPort} = require('serialport');
// const { ReadlineParser } = require('@serialport/parser-readline')
// const Chart = require('chart.js');
const path = require('path');
const fs = require('fs');
const { spawn } = require('child_process');
let pythonProcess = null;   //required for invoking python scripts

const app = express();
const server = http.createServer(app);
const io = socketIo(server);

const port = 8000;
// const serialPortPath = path.join('\\\\.\\COM19'); // Change this to your serial port path

// Setup Serial Port
// const portOptions = { baudRate: 9600 }; // Change baudRate if necessary
// const com = new SerialPort({
//     path:serialPortPath,
//     baudRate: 9600,
// });
// const parser = com.pipe(new ReadlineParser({ delimiter: '\n' }));

// Serve HTML file
app.get('/', (req, res) => {
    res.sendFile(__dirname + '/index.html');
});
app.get('/index.css', (req, res) => {
    res.sendFile(__dirname + '/index.css');
});
app.get('/index.js', (req, res) => {
    res.sendFile(__dirname + '/index.js');
});

app.get('/chart.js', (req, res) => {
    res.sendFile(__dirname + '/chart.js');
});


app.get('/socketed.js', (req, res) => {
    res.sendFile(__dirname + '/socketed.js');
});

app.get('/human.png', (req, res) => {
    res.sendFile(__dirname + '/human.png');
});

// Initialize data array for sensor values
let sensorData = [];

// Setup Socket.io connection
io.on('connection', (socket) => {
    console.log('A client connected');
    // Send initial data to client
    socket.emit('initialData', sensorData);
    
    socket.on('analyser', (data) => {
        // const ecg_params = `data: ${data}`;
        // console.log(ecg_params);
        const dataJSON = JSON.parse(data);
        const RA_factor = calculateReferenceFactor(dataJSON.RA[0], dataJSON.RA[1]);
        const LA_factor = calculateReferenceFactor(dataJSON.LA[0], dataJSON.LA[1]);
        const LL_factor = calculateReferenceFactor(dataJSON.LL[0], dataJSON.LL[1]);
        console.log(RA_factor, LA_factor, LL_factor);
        
        startECGAnalysis(RA_factor, LA_factor, LL_factor);
    });

    socket.on('halt', (data) => {
        const ecg_stop = `data: ${data}`;
        stopECGAnalysis();
        console.log("stopped!");
    });
});


// Function to log data to file
function logDataToFile(avrg, value1, value2) {
    const timestamp = new Date().toLocaleTimeString();
    const logEntry = `${timestamp}, ${avrg},${value1}, ${value2}, ${temperature}\n`;
    fs.appendFile('piezo-analog4.txt', logEntry, (err) => {
        if (err) {
            console.error('Error logging data to file:', err);
        } else {
            console.log('Data logged to file:', logEntry);
        }
    });
}

// Function to start the Python script
function startECGAnalysis(RA_val, LA_val, LL_val) {
    stopECGAnalysis();
    pythonProcess = spawn('python', ['../ECG_gene_and_analysis/ecg_generator.py', RA_val, LA_val, LL_val]);

    pythonProcess.stdout.on('data', (data) => {
        console.log(`Python script output: ${data}`);
    });

    pythonProcess.stderr.on('data', (data) => {
        console.error(`Python script error: ${data}`);
    });

    pythonProcess.on('close', (code) => {
        console.log(`Python script exited with code ${code}`);
    });
}

// Function to stop the Python script
function stopECGAnalysis() {
    if (pythonProcess) {
        pythonProcess.kill();
        pythonProcess = null;
        console.log('Python script stopped.');
    } else {
        console.log('Python script is not running.');
    }
}

// Start server
server.listen(port, () => {
    console.log(`Server running on http://localhost:${port}`);
});

function calculateReferenceFactor(x,y)
{
    const x_ref = 382;
    const y_ref = 166;
    

    const x_diff = Math.abs(x_ref - x);
    const y_diff = Math.abs(y_ref - y);
    // console.log("x=", x_diff, "y=", y_diff);
    const hypotenuse = Math.sqrt(Math.abs((x_diff*x_diff) - (y_diff*y_diff)));
    console.log("hypo", hypotenuse);

    let ref_factor = ((-1.0*hypotenuse)/200) + 1.0;
    if(ref_factor > 0.99) ref_factor = 0.95;

    return ref_factor;
}

