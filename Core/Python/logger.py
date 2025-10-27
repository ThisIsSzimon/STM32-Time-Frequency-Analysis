import serial
import csv
import os
import time

# --- KONFIGURACJA PORTU I PLiku ---
PORT = "COM3"           # dopasuj do swojego portu STM32
BAUD = 921600           # zgodne z ustawieniem w MX_USART2_UART_Init()

# --- ŚCIEŻKA DO PLIKU ---
base_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(base_dir, "data")
os.makedirs(data_dir, exist_ok=True)

csv_path = os.path.join(data_dir, f"dane_ascii.csv")

# --- KONFIGURACJA UART ---
ser = serial.Serial(PORT, BAUD, timeout=0.1)
ser.reset_input_buffer()

# --- ZAPIS DO CSV ---
with open(csv_path, mode="w", newline="") as file:
    w = csv.writer(file)
    w.writerow(["X", "Y", "Z"])

    written = 0
    try:
        print(f"[INFO] Zapis do: {csv_path}  (PORT={PORT}, BAUD={BAUD})")

        while True:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if not line:
                continue

            parts = line.split(",")
            if len(parts) != 3:
                continue

            try:
                x = int(parts[0])
                y = int(parts[1])
                z = int(parts[2])
            except ValueError:
                continue

            w.writerow([x, y, z])
            written += 1
            if written % 256 == 0:
                file.flush()

            # podgląd na żywo (opcjonalnie)
            if written % 1000 == 0:
                print(f"[INFO] Zapisano {written} próbek")

    except KeyboardInterrupt:
        print(f"\n[INFO] Zakończono. Zapisanych wierszy: {written}")
    finally:
        ser.close()
