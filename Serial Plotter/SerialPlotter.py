import serial
import tkinter as tk
from tkinter import scrolledtext
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import threading
import time
import pyaudio
import numpy as np
import queue

class SerialMonitor:

    def __init__(self, master):
        self.master = master
        self.master.title("Serial Monitor and Plotter")
        
        self.ser = serial.Serial('COM3', 115200, timeout=1)
        
        self.analog_data = []
        self.digital_data = []
        self.digital_timestamps = []
        self.timestamps = []
        self.last_digital_value = 0
        self.paused = False
        self.sound_enabled = False
        self.current_note = None
        self.note_queue = queue.Queue()  # Coda per le note da suonare
        
        self.setup_gui()
        
        self.running = True
        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.start()
        
        self.setup_audio()
        
        self.audio_thread = threading.Thread(target=self.sound_playback)
        self.audio_thread.start()

    def setup_gui(self):
        # Console
        self.console = scrolledtext.ScrolledText(self.master, wrap=tk.WORD, width=60, height=10)
        self.console.pack(padx=10, pady=10)
        
        # Input field and send button
        self.input_frame = tk.Frame(self.master)
        self.input_frame.pack(padx=10, pady=5)
        
        self.input_field = tk.Entry(self.input_frame, width=50)
        self.input_field.pack(side=tk.LEFT)
        
        self.send_button = tk.Button(self.input_frame, text="Send", command=self.send_command)
        self.send_button.pack(side=tk.LEFT)
        
        # Toggle sound button
        self.sound_button = tk.Button(self.master, text="Sound OFF", command=self.toggle_sound)
        self.sound_button.pack(pady=10)
        
        # Graphs
        self.fig, (self.ax1, self.ax2) = plt.subplots(2, 1, figsize=(8, 8))
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.master)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(padx=10, pady=10)
        
        self.ax1.set_title("Analog Values")
        self.ax2.set_title("Digital Values")
        
        self.line1, = self.ax1.plot([], [])
        self.line2, = self.ax2.step([], [], where='post')

    def setup_audio(self):
        # Frequenze delle 88 note del pianoforte
        self.notes = [self.note_frequency(i) for i in range(88)]
        self.audio = pyaudio.PyAudio()
        self.stream = self.audio.open(format=pyaudio.paFloat32, channels=1, rate=44100, output=True)

    def note_frequency(self, n):
        return 440.0 * (2.0 ** ((n - 69) / 12.0))  

    def sound_playback(self):
        while self.running:
            if self.sound_enabled:
                if not self.note_queue.empty():
                    self.current_note = self.note_queue.get()
                    tone = self.generate_tone(self.current_note)
                    self.stream.write(tone)
                else:
                    time.sleep(0.01) 
            else:
                time.sleep(0.1) 

    def generate_tone(self, freq, duration=0.1, sample_rate=44100):
        t = np.linspace(0, duration, int(sample_rate * duration), False)
        tone = 0.5 * np.sin(2 * np.pi * freq * t)
        return tone.astype(np.float32).tobytes()

    def toggle_sound(self):
        self.sound_enabled = not self.sound_enabled
        self.sound_button.config(text="Sound ON" if self.sound_enabled else "Sound OFF")
        if not self.sound_enabled:
            self.current_note = None  

    def read_serial(self):
        while self.running:
            if self.ser.in_waiting:
                line = self.ser.readline().decode('utf-8').strip()
                self.console.insert(tk.END, line + '\n')
                self.console.see(tk.END)
                
                current_time = time.time()
                
                if line == "WARNING":
                    self.paused = True
                    self.last_digital_value = 0
                    self.update_plots(current_time)
                elif not self.paused:
                    if line.startswith("Analog:"):
                        value = float(line.split(":")[1].strip())
                        self.analog_data.append(value)
                        self.timestamps.append(current_time)
                        if self.sound_enabled and self.last_digital_value == 1:
                            self.update_current_note(value)  # Aggiorna la nota corrente
                    elif line.startswith("Digital:"):
                        value = int(line.split(":")[1].strip())
                        if self.last_digital_value != value:  # Cambia solo se c'è un cambiamento
                            self.digital_data.append(value)
                            self.digital_timestamps.append(current_time)
                            self.last_digital_value = value
                            if self.sound_enabled and value == 1:
                                self.update_current_note(self.analog_data[-1])  # Aggiorna la nota se il segnale digitale è 1
                            elif value == 0:
                                self.current_note = None  # Ferma il suono se il segnale digitale è 0
                    self.update_plots(current_time)
                else:  # Se è in pausa, controlla se è il momento di riprendere
                    if line.startswith("Analog:") or line.startswith("Digital:"):
                        self.paused = False
                        # Processa il nuovo dato
                        if line.startswith("Analog:"):
                            value = float(line.split(":")[1].strip())
                            self.analog_data.append(value)
                            self.timestamps.append(current_time)
                        elif line.startswith("Digital:"):
                            value = int(line.split(":")[1].strip())
                            self.digital_data.append(value)
                            self.digital_timestamps.append(current_time)
                            self.last_digital_value = value
                        self.update_plots(current_time)

    def update_current_note(self, analog_value):
        # Mappa il valore analogico a una delle 88 note
        index = int(analog_value / (4050 / 88))
        index = min(max(index, 0), 87)  # Assicurati che sia tra 0 e 87
        self.note_queue.put(self.notes[index])  # Aggiungi la nota alla coda

    def update_plots(self, current_time):
        # Aggiorna il grafico analogico
        if len(self.timestamps) > 100:
            self.timestamps = self.timestamps[-100:]
            self.analog_data = self.analog_data[-100:]
        
        self.line1.set_data(self.timestamps, self.analog_data)
        self.ax1.relim()
        self.ax1.autoscale_view()
        
        # Aggiorna il grafico digitale
        if self.digital_data:
            # Aggiungi un punto extra per mantenere l'ultimo valore
            extended_timestamps = self.digital_timestamps + [current_time]
            extended_data = self.digital_data + [self.last_digital_value]
            
            self.line2.set_data(extended_timestamps, extended_data)
            self.ax2.set_ylim(-0.1, 1.1)  # Imposta i limiti dell'asse y tra -0.1 e 1.1
            self.ax2.relim()
            self.ax2.autoscale_view(scalex=True, scaley=False)  # Autoscale solo sull'asse x
        
        self.canvas.draw()

    def send_command(self):
        command = self.input_field.get()
        self.ser.write((command + '\n').encode())
        self.input_field.delete(0, tk.END)

    def on_closing(self):
        self.running = False
        self.ser.close()
        self.stream.stop_stream()
        self.stream.close()
        self.audio.terminate()
        self.master.quit()
        self.master.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialMonitor(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()
