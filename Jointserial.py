import tkinter as tk
import threading
import serial
import time
import queue

class SerialComm(threading.Thread):
    def __init__(self, port, baud_rate, get_message_func, status_queue):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.get_message_func = get_message_func
        self.status_queue = status_queue
        self.running = True
        self.ser = None

    def run(self):
        try:
            self.ser = serial.Serial(self.port, self.baud_rate, timeout=1)
            time.sleep(2)  # Allow time for the connection to initialize
            self.status_queue.put("Status: Serial port opened successfully.")
        except Exception as e:
            self.status_queue.put(f"Status: Failed to open serial port: {e}")
            return

        while self.running:
            # Send the message
            message = self.get_message_func()
            try:
                self.ser.write(message.encode('utf-8'))
                self.status_queue.put(f"Sent: {message.strip()}")
            except Exception as e:
                self.status_queue.put(f"Status: Error writing to serial port: {e}")

            # Check for incoming data
            try:
                if self.ser.in_waiting > 0:
                    data = self.ser.readline().decode('utf-8', errors='replace')
                    if data:
                        self.status_queue.put(f"Received: {data.strip()}")
            except Exception as e:
                self.status_queue.put(f"Status: Error reading from serial port: {e}")

            time.sleep(1)

        if self.ser and self.ser.is_open:
            self.ser.close()
            self.status_queue.put("Status: Serial port closed.")

def main():
    root = tk.Tk()
    root.title("Serial Communication GUI")
    
    # Queue for status and received messages
    status_queue = queue.Queue()

    # Tkinter variables for the data and serial settings
    j1_var = tk.StringVar(value="250.0")
    j2_var = tk.StringVar(value="500.5")
    j3_var = tk.StringVar(value="750.4")
    j4_var = tk.StringVar(value="310.2")
    # On Ubuntu, change the default port if needed (e.g., /dev/ttyUSB0 or /dev/ttyACM0)
    port_var = tk.StringVar(value="/dev/ttyUSB0")
    baud_var = tk.StringVar(value="115200")

    comm_thread = None

    # Create GUI elements
    tk.Label(root, text="j1:").grid(row=0, column=0, sticky="e")
    tk.Entry(root, textvariable=j1_var).grid(row=0, column=1)

    tk.Label(root, text="j2:").grid(row=1, column=0, sticky="e")
    tk.Entry(root, textvariable=j2_var).grid(row=1, column=1)

    tk.Label(root, text="j3:").grid(row=2, column=0, sticky="e")
    tk.Entry(root, textvariable=j3_var).grid(row=2, column=1)

    tk.Label(root, text="j4:").grid(row=3, column=0, sticky="e")
    tk.Entry(root, textvariable=j4_var).grid(row=3, column=1)

    tk.Label(root, text="Serial Port:").grid(row=4, column=0, sticky="e")
    tk.Entry(root, textvariable=port_var).grid(row=4, column=1)

    tk.Label(root, text="Baud Rate:").grid(row=5, column=0, sticky="e")
    tk.Entry(root, textvariable=baud_var).grid(row=5, column=1)

    # Status label (shows the most recent status message)
    status_label = tk.Label(root, text="Status: Idle", fg="blue")
    status_label.grid(row=6, column=0, columnspan=2, pady=(5, 10))

    # Text widget for displaying all messages (both sent and received)
    received_text = tk.Text(root, height=10, width=50)
    received_text.grid(row=7, column=0, columnspan=2, pady=(5,10))
    # Configure tags for sent and received messages
    received_text.tag_configure("sent", foreground="blue")
    received_text.tag_configure("received", foreground="green")

    def poll_queue():
        try:
            while True:
                message = status_queue.get_nowait()
                # Determine tag based on message type
                if message.startswith("Sent:"):
                    tag = "sent"
                elif message.startswith("Received:"):
                    tag = "received"
                else:
                    tag = None
                if tag:
                    received_text.insert(tk.END, message + "\n", tag)
                else:
                    received_text.insert(tk.END, message + "\n")
                received_text.see(tk.END)
                status_label.config(text=message)
        except queue.Empty:
            pass
        root.after(100, poll_queue)

    root.after(100, poll_queue)

    def get_message():
        # Construct the message from current variable values
        return f"j1={j1_var.get()},j2={j2_var.get()},j3={j3_var.get()},j4={j4_var.get()}\n"

    def start_comm():
        nonlocal comm_thread
        if comm_thread is None or not comm_thread.is_alive():
            port = port_var.get()
            try:
                baud_rate = int(baud_var.get())
            except ValueError:
                status_queue.put("Status: Invalid baud rate")
                return
            comm_thread = SerialComm(port, baud_rate, get_message, status_queue)
            comm_thread.start()
            status_queue.put("Status: Communication started.")

    def stop_comm():
        nonlocal comm_thread
        if comm_thread and comm_thread.is_alive():
            comm_thread.running = False
            comm_thread.join()
            status_queue.put("Status: Communication stopped.")
            comm_thread = None

    start_button = tk.Button(root, text="Start", command=start_comm)
    start_button.grid(row=8, column=0, padx=5, pady=5)
    stop_button = tk.Button(root, text="Stop", command=stop_comm)
    stop_button.grid(row=8, column=1, padx=5, pady=5)

    def on_closing():
        stop_comm()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == "__main__":
    main()
