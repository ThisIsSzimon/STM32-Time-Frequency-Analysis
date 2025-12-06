import os
import csv

# ---- konfiguracja ----
FS_WRONG = 3200.0
FS_TRUE = 1600.0
SCALE = FS_WRONG / FS_TRUE  # = 2.0

# katalog bazowy = folder, gdzie leży ten skrypt
base_dir = os.path.dirname(os.path.abspath(__file__))

root_dir = os.path.join(
    base_dir,
    "data",
    "pomiary_finalne",
    "fabryczne_nieuszkodzone"
)

pwm_dirs = ["pwm_20%", "pwm_30%"]  # podkatalogi do poprawy

def fix_file(in_path):
    name, ext = os.path.splitext(os.path.basename(in_path))
    out_path = os.path.join(os.path.dirname(in_path), f"{name}_fs1600{ext}")

    print(f"[INFO] Przetwarzam: {in_path}")
    print(f"[INFO] Zapis do:   {out_path}")

    with open(in_path, "r", newline="", encoding="utf-8") as fin, \
         open(out_path, "w", newline="", encoding="utf-8") as fout:

        r = csv.reader(fin)
        w = csv.writer(fout)

        # nagłówek przepisujemy 1:1
        header = next(r, None)
        if header is not None:
            w.writerow(header)

        for row in r:
            if not row:
                continue  # pusta linia

            try:
                t_old = float(row[0])
            except ValueError:
                # jeśli pierwsza kolumna nie jest liczbą (np. jakiś dziwny wiersz) – przepisz bez zmian
                w.writerow(row)
                continue

            t_new = t_old * SCALE
            # zachowaj resztę kolumn bez zmian
            new_row = [f"{t_new:.6f}"] + row[1:]
            w.writerow(new_row)

for pwm_dir in pwm_dirs:
    folder = os.path.join(root_dir, pwm_dir)
    if not os.path.isdir(folder):
        print(f"[WARN] Folder nie istnieje: {folder}")
        continue

    for fname in os.listdir(folder):
        if fname.lower().endswith(".csv"):
            in_file = os.path.join(folder, fname)
            fix_file(in_file)

print("[INFO] Gotowe. Oryginalne pliki zostały zachowane, nowe mają sufiks _fs1600.")
