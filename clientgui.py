from parse import compile
import socket
from tkinter import *
from tkinter import ttk
from ttkthemes import ThemedTk

entries = []
buttons = []
lables = []


def from_rgb(rgb):
    """translates an rgb tuple of int to a tkinter friendly color code
    """
    return "#%02x%02x%02x" % rgb


def updateIp(r):
    col = from_rgb((20, 20, 20))
    if not hasattr(updateIp, "odd"):
        updateIp.odd = False

    if updateIp.odd == True:
        col = from_rgb((35, 35, 35))
    print(updateIp.odd)
    updateIp.odd = not updateIp.odd
    e = ttk.Entry(ip_frame)
    e.insert(0, " ")
    e.pack(padx=4, pady=2)
    entries.append(e)
    l = ttk.Label(output_frame,
                  text="NO OUTPUT", borderwidth=3, relief="solid")

    l.pack(padx=1, pady=1)
    lables.append(l)


p = compile("#loadinfo 12345[{},{},{},{},{}]\n")
port = 18000


def clamp(minimum, x, maximum):
    return max(minimum, min(x, maximum))


def pingServer(f):
    req_header = b'#loadinfo 12345'

    for entry, lab in zip(entries, lables):
        eip = entry.get()
        if (len(eip) < 8):
            continue
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1.0)
        eip = eip.replace(" ", "")
        print(f"ip={eip}")
        sock.connect((eip, port))
        sock.send(req_header)
        data = sock.recv(256)
        load = data.decode()
        m = p.parse(load)
        load_info = (float(m[0]), float(m[1]),
                     float(m[2]), int(m[3]), int(m[4]))
        print(load_info)
        out_str = f"{eip} :avg 1 mins:{load_info[0]}, avg 5 mins:{load_info[1]}, avg 15 mins:{load_info[2]}, threads:{load_info[3]}/{load_info[4]}"
        avg = (load_info[0]+load_info[1]*0.25+load_info[2]*0.06)
        green = 255 - clamp(0, avg*15, 255)
        red = clamp(0, avg*20, 255)
        lab.config(text=out_str)
        lab.config(background=from_rgb((int(red), int(green), 25)))


isdd = True
root = ThemedTk(theme='blue')
style = ttk.Style()

print(root.get_themes())
root.configure(bg=from_rgb((10, 10, 30)))
root.geometry("600x600")
buttons.append(
    ttk.Button(root, text="Add IP", command=lambda: updateIp(root)).pack(padx=2, pady=2))

ping_server_btn = ttk.Button(root, text="Ping servers",
                             command=lambda: pingServer(output_frame))
ping_server_btn.pack(padx=2, pady=2)

ip_frame = ttk.Frame(root, borderwidth=2)
ip_frame.pack(padx=20, pady=20)
output_frame = ttk.Frame(root, borderwidth=2)
output_frame.pack(padx=0, pady=0)
root.mainloop()
