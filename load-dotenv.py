#!/usr/bin/env python3
from os.path import isfile

Import("env")
if not isfile(".env"):
    print('File .env not found')
    exit(0)

try:
    with open('.env', 'r') as f:
        envs = []
        for line in f:
            line = line.strip()
            if line.startswith("#"):
                continue
            envs.append(f'-DENV_{line}')
        env.Append(BUILD_FLAGS=envs)
except IOErrord:
    print('File .env. not accessible')
