import tkinter as tk
import threading
import serial
import time
import queue

class SerialSender(threading.Thread):
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
            self.status_queue.put("Serial port opened successfully.")
        except Exception as e:
            self.status_queue.put(f"Failed to open serial port: {e}")
            return

        while self.running:
            message = self.get_message_func()
            try:
                self.ser.write(message.encode('utf-8'))
                print("Sent:", message.strip())
            except Exception as e:
                self.status_queue.put(f"Error writing to serial port: {e}")
            time.sleep(1)

        if self.ser and self.ser.is_open:
            self.ser.close()
            self.status_queue.put("Serial port closed.")

def main():
    root = tk.Tk()
    root.title("Serial Sender GUI")
    
    # Create a thread-safe queue for status messages
    status_queue = queue.Queue()

    # Tkinter StringVars for the variables
    j1_var = tk.StringVar(value="250.0")
    j2_var = tk.StringVar(value="500.5")
    j3_var = tk.StringVar(value="750.4")
    j4_var = tk.StringVar(value="310.2")
    port_var = tk.StringVar(value="COM9")
    baud_var = tk.StringVar(value="115200")

    sender_thread = None

    # Layout the GUI
    tk.Label(root, text="j1:").grid(row=0, column=0, sticky="e")
    tk.Entry(root, textvariable=j1_var).grid(row=0, column=1)

    tk.Label(root, text="j2:").grid(row=1, column=0, sticky="e")
    tk.Entry(root, textvariable=j2_var).grid(row=1, column=1)

    tk.Label(root, text="j3:").grid(row=2, column=0, sticky="e")
    tk.Entry(root, textvariable=j3_var).grid(row=2, column=1)

    tk.Label(root, text="j4:").grid(row=3, column=0, sticky="e")
    tk.Entry(root, textvariable=j4_var).grid(row=3, column=1)

    tk.Label(root, text="COM Port:").grid(row=4, column=0, sticky="e")
    tk.Entry(root, textvariable=port_var).grid(row=4, column=1)

    tk.Label(root, text="Baud Rate:").grid(row=5, column=0, sticky="e")
    tk.Entry(root, textvariable=baud_var).grid(row=5, column=1)

    status_label = tk.Label(root, text="Status: Idle", fg="blue")
    status_label.grid(row=6, column=0, columnspan=2, pady=(5,10))

    def poll_queue():
        try:
            while True:
                message = status_queue.get_nowait()
                status_label.config(text="Status: " + message)
        except queue.Empty:
            pass
        root.after(100, poll_queue)

    root.after(100, poll_queue)

    def get_message():
        # Construct the message from current variable values
        return f"j1={j1_var.get()},j2={j2_var.get()},j3={j3_var.get()},j4={j4_var.get()}\n"

    def start_sending():
        nonlocal sender_thread
        # Prevent multiple sender threads from starting
        if sender_thread is None or not sender_thread.is_alive():
            port = port_var.get()
            try:
                baud_rate = int(baud_var.get())
            except ValueError:
                status_queue.put("Invalid baud rate")
                return
            sender_thread = SerialSender(port, baud_rate, get_message, status_queue)
            sender_thread.start()
            status_queue.put("Sending started.")

    def stop_sending():
        nonlocal sender_thread
        if sender_thread and sender_thread.is_alive():
            sender_thread.running = False
            sender_thread.join()
            status_queue.put("Sending stopped.")
            sender_thread = None

    start_button = tk.Button(root, text="Start", command=start_sending)
    start_button.grid(row=7, column=0, padx=5, pady=5)

    stop_button = tk.Button(root, text="Stop", command=stop_sending)
    stop_button.grid(row=7, column=1, padx=5, pady=5)

    def on_closing():
        stop_sending()
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == "__main__":
    main()
