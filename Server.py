#Code lines based on or gotten from AI is marked with "##" at the end.
#Whole functions based on or gotten from AI will be indicated by small text before the funntion. 

from flask import Flask, jsonify, render_template_string, request##
import serial##
import threading##
import time##
import socket##
import atexit##

sensor_data_lock = threading.Lock() ##

app = Flask(__name__)

# Serial data storage
sensor_data = {
    "co2": "No data",
    "temperature": "No data",
    "humidity": "No data",
    "window_state": "Unknown",
    "co2_open_threshold": "1000.0",
    "co2_close_threshold": "800.0",
    "temp_open_threshold": "27.0",
    "temp_close_threshold": "23.0",
    "humidity_open_threshold": "70.0",
    "humidity_close_threshold": "60.0"
}


try:
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0)
    time.sleep(2)  # Allow time for the serial connection to initialize
    atexit.register(ser.close)##
except serial.SerialException as e:
    print(f"Serial connection failed: {e}")
    ser = None

# Function to constantly read serial data
def read_serial():
    global sensor_data
    buffer = ''
    while True:
        if ser:
            try:
                while ser.in_waiting > 0:
                    buffer += ser.read(ser.in_waiting).decode('utf-8', errors='ignore')##
                    if '\n' in buffer:
                        lines = buffer.split('\n')
                        buffer = lines[-1]##
                        for line in lines[:-1]:##
                            line = line.strip()
                            if line.startswith("CO2:"):
                                parts = line.split(",")
                                if len(parts) >= 4:
                                    # Function to sanitize numerical strings
                                    def sanitize_num_str(s):##
                                        return s.strip().replace(',', '.')##
                                    co2_str = sanitize_num_str(parts[0].split(":")[1])##
                                    temp_str = sanitize_num_str(parts[1].split(":")[1])##
                                    humidity_str = sanitize_num_str(parts[2].split(":")[1])##
                                    with sensor_data_lock:##
                                        sensor_data["co2"] = co2_str
                                        sensor_data["temperature"] = temp_str
                                        sensor_data["humidity"] = humidity_str
                                        sensor_data["window_state"] = "Open" if "Open" in parts[3] else "Closed"
                            elif line.startswith("Thresholds:"):
                                # Parse thresholds from the serial output
                                thresholds = line[len("Thresholds:"):].split(",")
                                if len(thresholds) == 6:
                                    with sensor_data_lock:##
                                        sensor_data["co2_open_threshold"] = thresholds[0].strip()
                                        sensor_data["co2_close_threshold"] = thresholds[1].strip()
                                        sensor_data["temp_open_threshold"] = thresholds[2].strip()
                                        sensor_data["temp_close_threshold"] = thresholds[3].strip()
                                        sensor_data["humidity_open_threshold"] = thresholds[4].strip()
                                        sensor_data["humidity_close_threshold"] = thresholds[5].strip()
            except Exception as e:##
                print(f"Error reading serial: {e}")##
        time.sleep(0.1)##

# Start the serial reading in a separate thread

##Function based on or gotten from AI ##
if ser:
    serial_thread = threading.Thread(target=read_serial, daemon=True)
    serial_thread.start()


# Function to get the current IP address of the PC

##Function based on or gotten from AI##
def get_ip_address():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # This doesn't have to be reachable
        s.connect(("10.255.255.255", 1))
        ip = s.getsockname()[0]
    except Exception:
        ip = "127.0.0.1"
    finally:
        s.close()
    return ip


##whole function based on or gotten from AI##
@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        if 'command' in request.form:
            # Handle 'O' (Open) and 'C' (Close) commands
            command = request.form.get('command')
            if ser and command in ['O', 'C']:
                ser.write((command + '\n').encode('utf-8')
        elif 'set_timer' in request.form:
            # Handle timer command
            duration = request.form.get('duration', '0').strip()
            if duration.isdigit() and int(duration) > 0:
                command = f"Timer,{duration}\n"
                if ser:
                    ser.write(command.encode('utf-8'))
                    print(f"Sent timer command to Arduino: {command}")
                    time.sleep(0.2)
            else:
                print("Invalid timer duration.")
        elif 'cancel_override' in request.form:
            # Handle cancel override
            command = "CancelOverride\n"
            if ser:
                ser.write(command.encode('utf-8'))
                print("Sent cancel override command to Arduino.")
                time.sleep(0.2)
    return render_template_string("""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <title>Real-Time Actuator Control</title>
        <style>
            body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }
            h1 { color: #333; }
            .data { font-size: 18px; margin: 10px; }
            .control { margin: 20px; }
            button { padding: 10px 20px; font-size: 16px; margin: 5px; }
            input { padding: 5px; font-size: 16px; margin: 5px; width: 100px; }
            label { display: block; margin: 5px 0 5px; }
            .container { display: inline-block; text-align: left; }
        </style>
    </head>
    <body>
        <h1>Real-Time Actuator Control</h1>
        <!-- Display sensor data -->
        <div class="data">
            <p>CO2 Concentration: <span id="co2">{{ co2 }}</span></p>
            <p>Temperature: <span id="temperature">{{ temperature }}</span></p>
            <p>Humidity: <span id="humidity">{{ humidity }}</span> </p>
            <p>Window State: <span id="window_state">{{ window_state }}</span></p>
        </div>
        <!-- Control buttons -->
        <div class="control">
            <form method="post" style="display:inline;">
                <button name="command" value="O">Open Window</button>
                <button name="command" value="C">Close Window</button>
            </form>
            <!-- Timer form -->
            <form method="post" style="display:inline;">
                <label for="duration">Set Timer to Close Window (seconds):</label>
                <input type="text" name="duration" pattern="\d+" required>
                <button type="submit" name="set_timer">Set Timer</button>
            </form>
            <form method="post" style="display:inline;">
                <button type="submit" name="cancel_override">Cancel Timer</button>
            </form>
            <!-- Settings button -->
            <form action="/settings" method="get" style="display:inline;">
                <button type="submit">Settings</button>
            </form>
        </div>
        <!-- JavaScript for data fetching -->
        <script>
            function fetchData() {
                fetch('/data')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('co2').innerText = data.co2;
                        document.getElementById('temperature').innerText = data.temperature;
                        document.getElementById('humidity').innerText = data.humidity;
                        document.getElementById('window_state').innerText = data.window_state;
                    })
                    .catch(error => console.error('Error fetching data:', error));
            }
            setInterval(fetchData, 2000); // Fetch data every 2 seconds
        </script>
        <p>Access the app at: <strong>http://{{ ip }}:5000</strong></p>
    </body>
    </html>
    """, **sensor_data, ip=get_ip_address())


##whole function based on or gotten from AI##
@app.route('/data')
def data():
    with sensor_data_lock:
        return jsonify(sensor_data)



##whole function based on or gotten from AI##
@app.route('/settings', methods=['GET', 'POST'])
def settings():
    if request.method == 'POST':
        # Collect the updated thresholds from the form
        def getThresholds():
            co2_open_threshold = request.form.get('co2_open_threshold', sensor_data["co2_open_threshold"])
            co2_close_threshold = request.form.get('co2_close_threshold', sensor_data["co2_close_threshold"])
            temp_open_threshold = request.form.get('temp_open_threshold', sensor_data["temp_open_threshold"])
            temp_close_threshold = request.form.get('temp_close_threshold', sensor_data["temp_close_threshold"])
            humidity_open_threshold = request.form.get('humidity_open_threshold', sensor_data["humidity_open_threshold"])
            humidity_close_threshold = request.form.get('humidity_close_threshold', sensor_data["humidity_close_threshold"])
            return co2_open_threshold, co2_close_threshold, temp_open_threshold, temp_close_threshold, humidity_open_threshold, humidity_close_threshold
        
        a,b,c,d,e,f = getThresholds()
        sensor_data.update({
            "co2_open_threshold": a,   
            "co2_close_threshold": b,
            "temp_open_threshold": c,  
            "temp_close_threshold": d,
            "humidity_open_threshold": e,
            "humidity_close_threshold": f  
        })
            
            
        
        # Send updated thresholds to Arduino
        if ser:
            # Build the command string
            thresholds_string = f"T,{a},{b},{c},{d},{e},{f}\n"
            ser.write(thresholds_string.encode('utf-8'))
            
            print(f"Sent thresholds to Arduino: {thresholds_string}")
            time.sleep(0.2)
        
        # Confirmation message
        return render_template_string("""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <title>Settings Updated</title>
        </head>
        <body>
            <h1>Thresholds Update Sent to Arduino!</h1>
            <p>New thresholds have been applied.</p>
            <a href="/settings">Back to Settings</a> | <a href="/">Return to Main Page</a>
        </body>
        </html>
        """)
    else:
        # Render settings page with current values
        return render_template_string("""
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <title>Update Thresholds</title>
            <style>
                body { font-family: Arial, sans-serif; text-align: center; margin: 20px; }
                h1 { color: #333; }
                .thresholds { font-size: 18px; margin: 10px; }
                input { padding: 5px; font-size: 16px; margin: 5px; width: 200px; }
                label { display: block; margin: 5px 0 5px; }
                button { padding: 10px 20px; font-size: 16px; margin: 5px; }
                .container { display: inline-block; text-align: left; }
            </style>
        </head>
        <body>
            <h1>Update Thresholds</h1>
            <p>Please use a period (.) as the decimal separator.</p>
            <div class="thresholds">
                <div class="container">
                    <form method="post">
                        <label for="co2_open_threshold">CO2 Open Threshold (ppm):</label>
                        <input type="text" pattern="[0-9]+([\.][0-9]+)?" name="co2_open_threshold" value="{{ co2_open_threshold }}" placeholder="e.g., 1200.0" required><br>
                        <label for="co2_close_threshold">CO2 Close Threshold (ppm):</label>
                        <input type="text" pattern="[0-9]+([\.][0-9]+)?" name="co2_close_threshold" value="{{ co2_close_threshold }}" placeholder="e.g., 800.0" required><br>
                        <label for="temp_open_threshold">Temperature Open Threshold (°C):</label>
                        <input type="text" pattern="[0-9]+([\.][0-9]+)?" name="temp_open_threshold" value="{{ temp_open_threshold }}" placeholder="e.g., 27.0" required><br>
                        <label for="temp_close_threshold">Temperature Close Threshold (°C):</label>
                        <input type="text" pattern="[0-9]+([\.][0-9]+)?" name="temp_close_threshold" value="{{ temp_close_threshold }}" placeholder="e.g., 23.0" required><br>
                        <label for="humidity_open_threshold">Humidity Open Threshold (%):</label>
                        <input type="text" pattern="[0-9]+([\.][0-9]+)?" name="humidity_open_threshold" value="{{ humidity_open_threshold }}" placeholder="e.g., 70.0" required><br>
                        <label for="humidity_close_threshold">Humidity Close Threshold (%):</label>
                        <input type="text" pattern="[0-9]+([\.][0-9]+)?" name="humidity_close_threshold" value="{{ humidity_close_threshold }}" placeholder="e.g., 60.0" required><br><br>
                        <button type="submit" name="update_thresholds" value="Update">Update Thresholds</button>
                    </form>
                </div>
            </div>
            <a href="/">Return to Main Page</a>
        </body>
        </html>
        """, **sensor_data)

if __name__ == "__main__":
    ip_address = get_ip_address()
    print(f"Server is running. Access it at: http://{ip_address}:5000")
    app.run(host="0.0.0.0", port=5000, debug=True, use_reloader=False)

