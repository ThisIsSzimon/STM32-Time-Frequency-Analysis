import serial
import csv
import os

# USTAW to na swój port i baud:
PORT = "COM3"        # sprawdź w Menedżerze urządzeń (może u Ciebie COM6)
BAUD = 921600        # musi się zgadzać z MX_USART2_UART_Init()

# Ścieżka: Core/Python/data/nowe_dane.csv
base_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(base_dir, 'data')
os.makedirs(data_dir, exist_ok=True)
csv_path = os.path.join(data_dir, 'nowe_dane.csv')  # <- NADPISYWANIE

ser = serial.Serial(PORT, BAUD, timeout=1)
ser.reset_input_buffer()  # wyczyść śmieci z bufora na starcie

with open(csv_path, mode='w', newline='') as file:
    w = csv.writer(file)
    w.writerow(['X', 'Y', 'Z'])
    written = 0
    try:
        print(f"[INFO] Zapis do: {csv_path}  (PORT={PORT}, BAUD={BAUD})")
        while True:
            # czytamy do \n (na MCU wysyłamy \r\n, pyserial utnie CR)
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if not line:
                continue

            print(line)  # echo do terminala, żeby widzieć surowe linie

            parts = line.split(',')
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
            file.flush()  # flush po każdej linii (pewny zapis)

    except KeyboardInterrupt:
        print(f"\n[INFO] Zakończono. Zapisanych wierszy: {written}")
        file.flush()
    finally:
        ser.close()
