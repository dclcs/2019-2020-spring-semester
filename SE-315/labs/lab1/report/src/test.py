#!/usr/bin/env python

import os
import random

counter = 0
count = int(input("How many tests would you like to take? >>> "))

success = 0
failure = 0

while counter < count:
    random_int = random.randint(0, 4294967295)
    random_base = random.randint(2, 16)
    random_sign = random.randint(0, 1)
    real_int = random_int if random_sign == 0 else -random_int

    bash = "echo %d %d %d | ./base_conv" % (random_int,
                                            random_base, random_sign)

    cont = os.popen(bash).readlines()[0]

    if int(cont, base=random_base) == real_int:
        # print("successfully convert %d to base %d: %s" %
            #   (random_int, random_base, cont))
        success += 1
    else:
        print("conversion error when convert %d to base %d: %s" %
              (random_int, random_base, cont))
        failure += 1
    counter += 1

print("All %d test(s) running over. %d passed and %d failed." %
      (count, success, failure))
