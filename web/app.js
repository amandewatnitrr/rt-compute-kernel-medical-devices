document.addEventListener('DOMContentLoaded', () => {
    const socket = io();
    const log = document.getElementById('log');
    const ctx = document.getElementById('hrChart').getContext('2d');
    const pauseBtn = document.getElementById('pauseBtn');
    const resumeBtn = document.getElementById('resumeBtn');
    const stopBtn = document.getElementById('stopBtn');
    const startBtn = document.getElementById('startBtn');

    let paused = false;
    let streamStopped = false;
    let bufferedMessages = [];
    let autoScroll = true;

    // Track user scroll position to disable autoscroll when not at bottom
    log.addEventListener('scroll', () => {
        const threshold = 10; // px tolerance
        autoScroll = (log.scrollTop + log.clientHeight >= log.scrollHeight - threshold);
    });

    const chart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: 'Heart Rate (BPM)',
                data: [],
                borderColor: 'rgba(255, 99, 132, 1)',
                backgroundColor: 'rgba(255, 99, 132, 0.2)',
                borderWidth: 2,
                fill: true,
                tension: 0.4
            }]
        },
        options: {
            scales: {
                x: {
                    type: 'linear',
                    title: {
                        display: true,
                        text: 'Time (Ticks)'
                    }
                },
                y: {
                    beginAtZero: false,
                    suggestedMin: 60,
                    suggestedMax: 90,
                    title: {
                        display: true,
                        text: 'Heart Rate'
                    }
                }
            },
            animation: {
                duration: 200
            }
        }
    });

    function addData(label, data) {
        chart.data.labels.push(label);
        chart.data.datasets.forEach((dataset) => {
            dataset.data.push(data);
        });

        // Limit the number of data points
        if (chart.data.labels.length > 50) {
            chart.data.labels.shift();
            chart.data.datasets[0].data.shift();
        }
        chart.update();
    }

    function addLog(message) {
        const line = document.createElement('div');
        line.className = 'log-line';
        
        if (message.startsWith('--- Tick')) {
            line.classList.add('tick');
        } else if (message.includes('Task 0') || message.includes('VitalSigns')) {
            line.classList.add('task-0');
        } else if (message.includes('Task 1') || message.includes('DrugDelivery')) {
            line.classList.add('task-1');
        } else if (message.includes('Task 2') || message.includes('Display')) {
            line.classList.add('task-2');
        } else if (message.includes('Scheduler')) {
            line.classList.add('scheduler');
        } else if (message.includes('Mutex')) {
            line.classList.add('mutex');
        }

        line.textContent = message;
        log.appendChild(line);
        if (autoScroll) {
            log.scrollTop = log.scrollHeight; // Only autoscroll if user hasn't scrolled up
        }
    }

    socket.on('kernel_data', (data) => {
        if (paused) {
            bufferedMessages.push(data);
            return;
        }
        if (data.type === 'log') {
            // Split multi-line logs
            data.message.split('\\n').forEach(line => {
                if (line.trim()) addLog(line.trim());
            });
        } else if (data.type === 'data') {
            addData(data.tick, data.hr);
        } else if (data.type === 'log_entry') {
            addLog(data.message);
        }
    });

    // Pause logic
    pauseBtn.addEventListener('click', () => {
        paused = true;
        pauseBtn.style.display = 'none';
        resumeBtn.style.display = 'inline-block';
    });

    resumeBtn.addEventListener('click', () => {
        paused = false;
        resumeBtn.style.display = 'none';
        pauseBtn.style.display = 'inline-block';
        // Flush buffered messages
        bufferedMessages.forEach(data => {
            if (data.type === 'log') {
                data.message.split('\\n').forEach(line => {
                    if (line.trim()) addLog(line.trim());
                });
            } else if (data.type === 'data') {
                addData(data.tick, data.hr);
            } else if (data.type === 'log_entry') {
                addLog(data.message);
            }
        });
        bufferedMessages = [];
    });

    // Stop stream (stop spawning new data from backend)
    stopBtn.addEventListener('click', () => {
        if (streamStopped) return;
        socket.emit('stop_stream');
        streamStopped = true;
        stopBtn.style.display = 'none';
        startBtn.style.display = 'inline-block';
    });

    // Start stream (request backend to respawn process)
    startBtn.addEventListener('click', () => {
        if (!streamStopped) return;
        socket.emit('start_stream');
        streamStopped = false;
        startBtn.style.display = 'none';
        stopBtn.style.display = 'inline-block';
    });
});
