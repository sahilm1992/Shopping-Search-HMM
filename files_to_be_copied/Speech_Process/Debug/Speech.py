#!C:\Python34\python.exe
import subprocess
from webob import Response
#subprocess.call("SpeechRecognition.exe")
res = Response()
print("Content-type: text/html")
print("Access-Control-Allow-Origin:*")
print("Access-Control-Expose-Headers:Access-Control-Allow-Origin")

print("HELLO WELCOME TO SPEECH PROJECT")
#proc = subprocess.Popen(["SpeechRecognition.exe"], 
#                        stdout=subprocess.PIPE,
#                       )
#stdout_value = proc.communicate()[0] 
#print (str(repr(stdout_value)))