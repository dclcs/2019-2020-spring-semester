#!/usr/bin/env python3

from math import sin, pi
from time import sleep
from random import random

theta_rad = 20
theta = theta_rad * pi / 180
omega = 0.0

world_g = 9.80665
length = 1

while True:
    time_elapse = random() / 10 + 0.05
    theta += omega * time_elapse
    omega -= world_g / length * sin(theta) * time_elapse

    print("theta: %.2f omega: %.2f\r" % (theta * 180 / pi, omega))

    sleep(time_elapse)
