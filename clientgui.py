from cProfile import label
from curses import BUTTON1_DOUBLE_CLICKED
from decimal import Clamped

from parse import compile
import socket
from tkinter import *
from tkinter import ttk
from ttkthemes import ThemedTk
import tkinter 
import sv_ttk

import threading
import time
entries = []
buttons = []
lables = []

auto_ping = False

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
        green = 100 - clamp(0, avg*15, 100)
        red = clamp(0, avg*20, 254)
        lab.config(text=out_str)
        lab.config(background=from_rgb((int(red), int(green), 25)))
def keep_pinging():
    while(automatic_ping_checkbox_var.get() == 1):
        pingServer(1)
        time.sleep(0.3)
    
def start_pinging(f):
    x = threading.Thread(target=keep_pinging)
    x.start()


f= open("ips",mode='r+',encoding='utf8')

for i in range(4):
    lineText=f.readline()
    print(lineText)

f.close()    
isdd = True
root = tkinter.Tk()
style = ttk.Style()
sv_ttk.set_theme("dark")

#print(root.get_themes())
root.configure(bg=from_rgb((0, 50, 100)))
root.geometry("600x600")
root.title( "Server monitor")

buttons.append(
    ttk.Button(root, text="Add IP", command=lambda: updateIp(root)).pack(padx=2, pady=5))
ping_frame = ttk.Frame(root)
ping_frame.pack(padx=10,pady=10)
ping_server_btn = ttk.Button(ping_frame, text="Ping servers",
                             command=lambda: pingServer(output_frame))
ping_server_btn.pack(side=LEFT,padx=2,pady=2)
automatic_ping_checkbox_var =  tkinter.IntVar(ping_frame)

automatic_ping_checkbox = ttk.Checkbutton(ping_frame, text="automatic ping",command=lambda: start_pinging(automatic_ping_checkbox_var),variable=automatic_ping_checkbox_var)
automatic_ping_checkbox.pack(padx=2, pady=2)
ip_frame = ttk.Frame(root, borderwidth=2)
ip_frame.pack(padx=20, pady=20)

output_frame = ttk.Frame(root, borderwidth=2)
output_frame.pack(padx=0, pady=0)

root.mainloop()
