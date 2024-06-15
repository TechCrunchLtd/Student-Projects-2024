const { spawn } = require('child_process');

let pythonProcess = null;

// Function to start the Python script
function startPythonScript() {
    pythonProcess = spawn('python', ['ecg_generator.py']);

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
function stopPythonScript() {
    if (pythonProcess) {
        pythonProcess.kill();
        pythonProcess = null;
        console.log('Python script stopped.');
    } else {
        console.log('Python script is not running.');
    }
}

// Example: Start the Python script
startPythonScript();

// Example: Stop the Python script after 5 seconds (for demonstration)
setTimeout(stopPythonScript, 10000);
