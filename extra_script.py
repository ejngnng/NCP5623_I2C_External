Import("env")
import time
import os
import string

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

buildtime = time.strftime('%Y%m%d%H%M%S', time.localtime(time.time()))

target_filename = "./src/version.h"
target_name = "THISFIRMWARE"
target_version = "THISVERSION"

try:
    fp = open(target_filename, "r")
    content = fp.readlines()
    fp.close
    for lines in content:
        if target_name in lines:
            result = lines.split()
#            print("find reversion: %s" % result[2])
            fw_name = result[2]
        if target_version in lines:
            rs_version = lines.split()
            fw_version = rs_version[2]

except IOError:
    print("fail to read file %s" % target_filename)

#env.Replace(PROGNAME="firmware_%s" % buildtime)
#env.Replace(PROGNAME = "FGSR_%s" % defines.get("VERSION"))
env.Replace(PROGNAME = "%s_%s_%s" % (eval(fw_name), defines.get("HW"), eval(fw_version)))
