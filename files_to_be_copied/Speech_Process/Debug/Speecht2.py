#!C:\Python34\python.exe
import subprocess
import cgi, cgitb 
print("Access-Control-Allow-Origin:*")
print("Access-Control-Expose-Headers:Access-Control-Allow-Origin")
print("Content-type: text/html\n\n")
# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
word = form.getvalue('Word')
user  = form.getvalue('userName')
mode  = form.getvalue('mode')
recNum  = form.getvalue('recNum')
needFile=form.getvalue('needFile')
if(needFile):
	needFile =int(needFile)
modeInt =None

if(mode):
	modeInt =int(mode)
fo=open("sounds.txt","r+")
fileText = fo.read(100)
fo.close()
sounds = fileText.split(",")
if(needFile==1):
	for x in sounds:
		print (x+" ")
#subprocess.call("SpeechRecognition.exe")




#print (int(mode))
#print (word)
#print (user)
commandString="SpeechRecognition.exe " 
#commandString +="1 " +"2"
#commandString+=str( mode) + " " + str(user)
#print (commandString)
#print (commandString)
# 5 for testing
proc =None

if(modeInt and modeInt==5):
	proc = subprocess.Popen([commandString,mode ,user], 
						stdout=subprocess.PIPE,
					   )
# 10 for RECORDING
if(modeInt and modeInt==10):
	proc = subprocess.Popen([commandString,mode ,user,word,recNum], 
						stdout=subprocess.PIPE,
					   )
# 11 for training on the currently recorded
if(modeInt and modeInt==11):
	proc = subprocess.Popen([commandString,mode,user], 
						stdout=subprocess.PIPE,
					   )
if(proc):
	stdout_value = proc.communicate()[0]
	stdout_value=stdout_value.decode("ascii")
	stdout_value=stdout_value.replace("'","")
	print (repr(stdout_value))
