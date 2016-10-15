#!C:\Python34\python.exe
import subprocess
import shutil
import cgi, cgitb 
import os,sys
print("Access-Control-Allow-Origin:*")
print("Access-Control-Expose-Headers:Access-Control-Allow-Origin")
print("Content-type: text/html\n\n")
# Create instance of FieldStorage 		
form = cgi.FieldStorage() 
# Get data from fields

userName = form.getvalue('userNm')
files = form['file']

#print (userName)

#print(files)
homeFolder = os.getcwd() 
userFolder = homeFolder +"\\"+userName 

userInputFolder = userFolder +"\\"+"input" 
os.makedirs(userInputFolder, exist_ok=True)

usersoundFolder = userInputFolder +"\\sound" 
os.makedirs(usersoundFolder, exist_ok=True)

usersoundModifiedFolder = usersoundFolder +"\\modified" 
os.makedirs(usersoundModifiedFolder, exist_ok=True)


#print(usersoundModifiedFolder)
'''
for x in files:
	print (x.filename)
'''

for x in files:
	#print (x)
	dest = usersoundFolder+"\\"
	#print(dest)
	#fn = secure_filename(x.filename)
	fn= x.filename
	#print (dest)
	with open(dest + fn, 'wb') as f:
		#print(dest+fn)
		shutil.copyfileobj(x.file, f)
		f.close()
		#print ("done")
print ("Files Uploaded successfully")	
#commandString="SpeechRecognition.exe " 

'''
proc = subprocess.Popen([commandString,mode,user], 
						stdout=subprocess.PIPE,
					   )
if(proc):
	stdout_value = proc.communicate()[0]
	stdout_value=stdout_value.decode("ascii")
	stdout_value=stdout_value.replace("'","")
	print (repr(stdout_value))
'''
