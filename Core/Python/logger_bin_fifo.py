import serial, csv, os, struct, time

PORT = "COM3"
BAUD = 921600
FS = 1600 # częstotliwość próbkowania ADXL345

base_dir = os.path.dirname(os.path.abspath(__file__))
data_dir = os.path.join(base_dir, "data")
os.makedirs(data_dir, exist_ok=True)
csv_path = os.path.join(data_dir, f"pomiar_z_50.csv")

SYNC = b'\xAA\x55'

def find_sync(ser):
    state = 0
    while True:
        b = ser.read(1)
        if not b: 
            continue
        if state == 0 and b == SYNC[0:1]:
            state = 1
        elif state == 1 and b == SYNC[1:2]:
            return
        else:
            state = 0

def read_frame(ser):
    hdr = ser.read(1 + 4)          # N + start_id(4)
    if len(hdr) != 5:
        return None
    N = hdr[0]
    if N == 0 or N > 16:
        return None
    start_id = struct.unpack('<I', hdr[1:5])[0]
    payload_len = 6 * N
    payload = ser.read(payload_len + 1)      # +checksum
    if len(payload) != payload_len + 1:
        return None

    data = bytes([N]) + hdr[1:5] + payload[:-1]
    csum = payload[-1]
    if (sum(data) & 0xFF) != csum:
        return None

    xyz = struct.unpack('<' + 'hhh'*N, payload[:-1])
    samples = []
    sid = start_id
    for i in range(N):
        x = xyz[3*i + 0]; y = xyz[3*i + 1]; z = xyz[3*i + 2]
        t = (sid - start_id) / FS    # czas względny w sekundach w obrębie tej ramki
        samples.append((t, x, y, z))
        sid += 1
    return samples, start_id

def main():
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
    ser.reset_input_buffer()

    with open(csv_path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["czas [s]", "X", "Y", "Z"])
        total = 0
        lost_frames = 0
        t_offset = None

        try:
            print(f"[INFO] Zapis do: {csv_path}")
            find_sync(ser)

            while True:
                find_sync(ser)
                frame = read_frame(ser)
                if frame is None:
                    lost_frames += 1
                    continue
                (samples, start_id) = frame

                # ustal czas początkowy przy pierwszej ramce
                if t_offset is None:
                    t_offset = start_id / FS

                # zapisz próbki z czasem bezwzględnym (s)
                for (t_rel, x, y, z) in samples:
                    t_abs = (start_id / FS + t_rel) - t_offset
                    w.writerow([t_abs, x, y, z])

                total += len(samples)
                if total % 2000 == 0:
                    f.flush()
                    print(f"[INFO] Próbki: {total}   (utracone ramki: {lost_frames})")
        except KeyboardInterrupt:
            print(f"\n[INFO] Koniec. Zapisano próbek: {total}, utracone ramki: {lost_frames}")
        finally:
            ser.close()

if __name__ == "__main__":
    main()
