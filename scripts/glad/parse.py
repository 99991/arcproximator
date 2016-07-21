import re

with open("glad.h", "rb") as f:
    s = f.read().strip()

lines = s.split("\n")

lines = list(line for line in lines if "PFNGL" in line)
pfns = list(line for line in lines if "typedef" in line)
funcs = re.findall("glad_(gl[0-9a-zA-Z]+)", s)
funcs = sorted(list(set(funcs)))
pfns = sorted(pfns, key=lambda x: re.findall("PFNGL[0-9a-zA-Z]+", x)[0])

for pfn in pfns:
    print(pfn.replace("APIENTRYP", "AR_APIENTRYP"))

print("\n")

for func in funcs:
    print("AR_GLAPI " + "PFN" + func.upper() + "PROC ar_" + func + ";")

print("\n"*100)

for func in funcs:
    print("AR_LOAD(" + func + ", PFN" + func.upper() + "PROC)")

print("\n"*100)

for line in s.split("\n"):
    if "#define" in line:
        print(line)
