#!/usr/bin/env python3
import sys
import os

body = sys.stdin.read()

print("Status: 200 OK")
print("Content-Type: text/plain")
print()
print("REQUEST_METHOD =", os.environ.get("REQUEST_METHOD"))
print("CONTENT_LENGTH =", os.environ.get("CONTENT_LENGTH"))
print("CONTENT_TYPE =", os.environ.get("CONTENT_TYPE"))
print("BODY =", body)