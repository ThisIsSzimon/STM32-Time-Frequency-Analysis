

def read_frame(ser):
    hdr = ser.read(1 + 4)          # N' + start_id(4)