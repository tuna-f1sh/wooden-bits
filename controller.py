import Tkinter
import tkMessageBox
import urllib
import urllib2
import datetime
from Tkinter import *

class Application(Frame):
    def say_hi(self):
        print "fgdg there, everyone!"

    def createWidgets(self):
        self.QUIT = Button(self)
        self.QUIT["text"] = "QUIT"
        self.QUIT["fg"]   = "red"
        self.QUIT["bg"]   = "blue"
        self.QUIT["command"] =  self.quit
        self.QUIT.pack({"side": "right"})

        self.time = Button(self)
        self.time["text"] = "Set Time"
        self.time["bg"]   = "blue"
        self.time["command"] = time
        self.time.pack({"side": "left"})

        self.hi_there = Button(self)
        self.hi_there["text"] = "Show Temperatue"
        self.hi_there["command"] = temp
        self.hi_there.pack({"side": "left"})

    def __init__(self, master=None):
        Frame.__init__(self, master)
        self.pack()
        self.createWidgets()

def time():
    time = datetime.datetime.now()
    data = [ ('set' , 1), ('hour' , format(time.hour,'02')), ('min' , format(time.minute,'02')) ]
    data = urllib.urlencode(data)
    print(data)
    req = urllib2.Request('http://192.168.4.1/' + '?' + data)
    res = urllib2.urlopen(req, timeout=5)

def temp():
    data = [ ('set' , 3), ('hour' , 00), ('min' , 00) ]
    data = urllib.urlencode(data)
    print(data)
    req = urllib2.Request('http://192.168.4.1/' + '?' + data)
    res = urllib2.urlopen(req, timeout=5)

root = Tk()
app = Application(master=root)
app.master.title("Wooden Bits Controller")
app.master.height = 2000
app.master.width = 2000
app.master.propagate(0)
app.mainloop()
root.destroy()
