#!/usr/bin/env python

from math import cos, sin, radians, pi
from kicad.pcbnew.board import Board

center = (153, 100)
radius_led = 25
radius_c = 21
count = 12
step = 360 / (count) / 180.0 * pi

b = Board.load("NavigationThing.kicad_pcb")

for m in b.modules:
    ref = m.native_obj.GetReference()
    if ref.startswith("LED"):
        num = (int(ref[3:]) + count / 2) % count

        angle = 2 * pi - num * step
        m.x = center[0] + radius_led * cos(angle)
        m.y = center[1] + radius_led * sin(angle)
        m.rotation = 3 * pi / 2. - angle

        print("Diode: " + str(num))
        print("({0}, {1})".format(m.x, m.y))

    elif ref.startswith("C"):
        num = (int(ref[1:]) + count / 2) % count
        angle = (num - 1) * step
        m.x = center[0] + radius_c * cos(angle)
        m.y = center[1] + radius_c * sin(angle)
        m.rotation = 3 * pi / 2. - angle

        print("Capacitor: " + str(num))
        print("({0}, {1})".format(m.x, m.y))

b.save()

print("Done")
